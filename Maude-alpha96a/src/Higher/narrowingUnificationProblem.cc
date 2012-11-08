/*

    This file is part of the Maude 2 interpreter.

    Copyright 1997-2006 SRI International, Menlo Park, CA 94025, USA.

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

*/

//
//	Implementation for class NarrowingUnificationProblem.
//

//	utility stuff
#include "macros.hh"
#include "vector.hh"
#include "allSat.hh"
#include "bddUser.hh"

//	forward declarations
#include "interface.hh"
#include "core.hh"
#include "variable.hh"

//	interface class definitions
#include "symbol.hh"
#include "dagNode.hh"
#include "extensionInfo.hh"

//	core class definitions
#include "module.hh"
#include "sortBdds.hh"
#include "connectedComponent.hh"
#include "unificationContext.hh"
#include "freshVariableGenerator.hh"
#include "preEquation.hh"

//	variable class definitions
#include "variableDagNode.hh"
#include "variableTerm.hh"

//	higher class definitions
#include "narrowingVariableInfo.hh"
#include "narrowingUnificationProblem.hh"

NarrowingUnificationProblem::NarrowingUnificationProblem(PreEquation* preEquation,
							 DagNode* target,
							 const NarrowingVariableInfo& variableInfo,
							 FreshVariableGenerator* freshVariableGenerator,
							 bool odd)
  : preEquation(preEquation),
    nrPreEquationVariables(preEquation->getNrRealVariables()),
    variableInfo(variableInfo),
    freshVariableGenerator(freshVariableGenerator),
    odd(odd)
{
  Module* module = preEquation->getModule();
  firstTargetSlot = module->getMinimumSubstitutionSize();
  substitutionSize = firstTargetSlot + variableInfo.getNrVariables();
  //cout << "firstTargetSlot = " << firstTargetSlot << " substitutionSize = " << substitutionSize << endl;
  sortBdds = module->getSortBdds();
  //
  //	Initialize the sorted and unsorted solutions.
  //
  orderSortedUnifiers = 0;
  sortedSolution = new Substitution(substitutionSize);
  unsortedSolution = new UnificationContext(freshVariableGenerator, substitutionSize, odd);
  for (int i = 0; i < substitutionSize; ++i)
    {
      sortedSolution->bind(i, 0);  // so GC doesn't barf
      unsortedSolution->bind(i, 0);  // HACK
    }
  //
  //	Solve the underlying many-sorted unification problem.
  //
  viable = preEquation->getLhsDag()->computeSolvedForm(target, *unsortedSolution, pendingStack);
}

NarrowingUnificationProblem::NarrowingUnificationProblem(DagNode* lhs,
							 DagNode* rhs,
							 const NarrowingVariableInfo& variableInfo,
							 FreshVariableGenerator* freshVariableGenerator,
							 bool odd)
  : preEquation(0),
    nrPreEquationVariables(0),
    variableInfo(variableInfo),
    freshVariableGenerator(freshVariableGenerator),
    odd(odd)
{
  //cout << lhs << " =? " << rhs << endl;
  Module* module = lhs->symbol()->getModule();
  //cout << "module: " << module << endl;

  firstTargetSlot = module->getMinimumSubstitutionSize();
  //cout << "firstTargetSlot: " << firstTargetSlot << endl;
  //cout << "nr unification variables: " << variableInfo.getNrVariables() << endl;
  substitutionSize = firstTargetSlot + variableInfo.getNrVariables();

  sortBdds = module->getSortBdds();
  //
  //	Initialize the sorted and unsorted solutions.
  //
  orderSortedUnifiers = 0;
  sortedSolution = new Substitution(substitutionSize);
  unsortedSolution = new UnificationContext(freshVariableGenerator, substitutionSize, odd);
  for (int i = 0; i < substitutionSize; ++i)
    {
      sortedSolution->bind(i, 0);  // so GC doesn't barf
      unsortedSolution->bind(i, 0);  // HACK
    }
  //
  //	Solve the underlying many-sorted unification problem.
  //
  viable = lhs->computeSolvedForm(rhs, *unsortedSolution, pendingStack);
}

NarrowingUnificationProblem::~NarrowingUnificationProblem()
{
  delete orderSortedUnifiers;
  delete unsortedSolution;
  delete sortedSolution;
}

void
NarrowingUnificationProblem::markReachableNodes()
{
  {
    int nrFragile = sortedSolution->nrFragileBindings();
    for (int i = 0; i < nrFragile; i++)
      {
	DagNode* d = sortedSolution->value(i);
	if (d != 0)
	  d->mark();
      }
  }
}

bool
NarrowingUnificationProblem::findNextUnifier()
{
  if (!viable)
    return false;

  bool first = true;
  //
  //	Do we already have an unsorted solution that we can find another of assigning sorts to.
  //
  if (orderSortedUnifiers != 0)
    {
      if (orderSortedUnifiers->nextAssignment())
	goto good;
      else
	{
	  delete orderSortedUnifiers;
	  orderSortedUnifiers = 0;
	  first = false;
	}
    }
  //
  //	Keep extracting solved forms until we find one which has no cycles and which order-sortable.
  //	If we run out solved forms, we're done.
  //
  for(;;)
    {
      if (!pendingStack.solve(first, *unsortedSolution))
	return false;
      if (extractUnifier() && findOrderSortedUnifiers())
	{
#ifdef NO_ASSERT
	  (void) orderSortedUnifiers->nextAssignment();
#else
	  bool t = orderSortedUnifiers->nextAssignment();
	  Assert(t, "no first order sorted unifier");
#endif
	  break;
	}
      first = false;
    }

 good:
  if (!freeVariables.empty())
    bindFreeVariables();
  return true;
}

Sort*
NarrowingUnificationProblem::variableIndexToSort(int index)
{
  if (index < nrPreEquationVariables)
    {
      //
      //	Original PreEquation variable.
      //
      return safeCast(VariableSymbol*, preEquation->index2Variable(index)->symbol())->getSort();
    }
  if (index < substitutionSize)
    {
      Assert(index >= firstTargetSlot, "variable index in no man's land " << index);
      //
      //	Original dag variable.
      //
      return safeCast(VariableSymbol*, variableInfo.index2Variable(index - firstTargetSlot)->symbol())->getSort();
    }
  //
  //	Fresh variable introduced by unification.
  //
  return unsortedSolution->getFreshVariableSort(index);
}

void
NarrowingUnificationProblem::bindFreeVariables()
{
  //
  //	We go through the variables that were free in the unsorted unifier and bind each to a fresh variable, whose sort is either that
  //	calculated by the BDDs for a constrained variables, or the sort of the original variable for an unconstrained variable.
  //
  const Vector<Byte>& assignment = orderSortedUnifiers->getCurrentAssignment();
  int bddVar = sortBdds->getFirstAvailableVariable();
  int freshVariableCount = 0;

  FOR_EACH_CONST(i, NatSet, freeVariables)
    {
      //
      //	Get original sort of variable and its connected component.
      //
      int fv = *i;
      Sort* sort = variableIndexToSort(fv);
      ConnectedComponent* component = sort->component();

      Sort* newSort;
      if (sortConstrainedVariables.contains(fv))
	{
	  //
	  //	The index of the new sort is binary encoded by the assignments to the BDD variables for this free variable.
	  //
	  int nrBddVariables = sortBdds->getNrVariables(component->getIndexWithinModule());
	  int newSortIndex = 0;
	  for (int j = nrBddVariables - 1; j >= 0; --j)
	    {
	      newSortIndex <<= 1;
	      if (assignment[bddVar + j])
		++newSortIndex;
	    }
	  bddVar += nrBddVariables;
	  newSort = component->sort(newSortIndex);
	}
      else
	{
	  //
	  //	We have a free variable that didn't occur in any other variables binding; therefore its sort
	  //	isn't constrained and remains the same.
	  //
	  newSort = sort;
	}
      //
      //	Make a new fresh variable with correct sort and name.
      //
      //	By giving fv as the slot, we ensure that we can reinstantiate users of this variable for second
      //	and subsequent sorted solutions corresponding to our unsorted solution.
      //
      DagNode* newVariable = new VariableDagNode(freshVariableGenerator->getBaseVariableSymbol(newSort),
						 freshVariableGenerator->getFreshVariableName(freshVariableCount, odd),
						 fv);
      //
      //	Bind slot to new variable.
      //
      sortedSolution->bind(fv, newVariable);
      ++freshVariableCount;
    }
  //
  //	Now go through the substitution, instantiating anything that is bound to something other than a free variable
  //	on the new free variables.
  //
  for (int i = 0; i < substitutionSize; ++i)
    {
      if (DagNode* d = sortedSolution->value(i))  // might be an unused slot
	{
	  if (!(freeVariables.contains(i)))  // not a free variable
	    {
	      if (DagNode* d2 = d->instantiate(*sortedSolution))
		sortedSolution->bind(i, d2);
	    }
	}
    }
}

bool
NarrowingUnificationProblem::findOrderSortedUnifiers()
{
  //
  //	We use clone() rather than copy() because addNewVariable() may have
  //	increased the number of variables above the number of original variables.
  //
  sortedSolution->clone(*unsortedSolution);
  //
  //	For each free variable allocate a block of BDD variables based on the
  //	size of the connected component of its sort.
  //
  //	We may have a bigger substitution than we started with due to fresh variables
  //	being created to express unifiers in theories such as AC.
  //	
  int newSubstitutionSize = sortedSolution->nrFragileBindings();
  //
  //	We keep track of the first BDD variable used for each free variable.
  //
  Vector<int> realToBdd(newSubstitutionSize);
  int nextBddVariable = sortBdds->getFirstAvailableVariable();
  {
    FOR_EACH_CONST(i, NatSet, sortConstrainedVariables)
      {
	int fv = *i;
	realToBdd[fv] = nextBddVariable;
	Sort* sort = variableIndexToSort(fv);
	nextBddVariable += sortBdds->getNrVariables(sort->component()->getIndexWithinModule());
      }
  }
  //
  //	Make sure BDD package has enough variables allocated.
  //
  BddUser::setNrVariables(nextBddVariable);
  //
  //	Constrain fresh sort constrained variables to have sort indices in valid range.
  //
  Bdd unifier = bddtrue;
  {
    FOR_EACH_CONST(i, NatSet, sortConstrainedVariables)
      {
	int fv = *i;
	if (fv >= substitutionSize)
	  {
	    Sort* sort = unsortedSolution->getFreshVariableSort(fv);
	    int nrBddVariables = sortBdds->getNrVariables(sort->component()->getIndexWithinModule());
	    int firstVar = realToBdd[fv];
	    
	    bddPair* bitMap = bdd_newpair();
	    for (int j = 0; j < nrBddVariables; ++j)
	      bdd_setbddpair(bitMap, j, bdd_ithvar(firstVar + j));
	    
	    Bdd leqRelation = sortBdds->getLeqRelation(sort->getIndexWithinModule());
	    leqRelation = bdd_veccompose(leqRelation, bitMap);
	    unifier = bdd_and(unifier, leqRelation);
	    DebugAdvisory("NarrowingUnificationProblem::findOrderSortedUnifiers() : Adding constraint for free, non-original variable: "
			  << leqRelation << " unifier becomes " << unifier);
	    bdd_freepair(bitMap);
	  }
      }
  }
  //
  //	Now compute a BDD which tells us if a given assignment of sorts to free
  //	variables yields an order-sorted unifier.
  //
  for (int i = 0; i < substitutionSize; ++i)
    {
      //
      //	We don't even look at fresh variables - they are not subject to original constraints - they get implicity constrained
      //	by where they show up if they are free. Therefore each slot we examine corresponds to:
      //	(1) an original variable that was bound;
      //	(2) an original variable that is free but sort constrained by its occurrence in bindings to other variables
      //	(3) an original variable that is free and doesn't occur in bindings to other variables; or
      //	(4) an unused slot lying between the preEquation variables and the dag variables (no man's land).
      //
      //	Cases (3) and (4) place no role in the BDD based sort computations.
      //
      DagNode* d = sortedSolution->value(i);
      if (d != 0 || sortConstrainedVariables.contains(i))
	{
	  bddPair* bitMap = bdd_newpair();
	  Sort* sort = variableIndexToSort(i);
	  Bdd leqRelation = sortBdds->getLeqRelation(sort->getIndexWithinModule());
	  if (d != 0)
	    {
	      //
	      //	Bound variable: term must have sort <= variables sort.
	      //
	      Vector<Bdd> genSort;
	      d->computeGeneralizedSort(*sortBdds, realToBdd, genSort);	  
	      int nrBdds =  genSort.size();
	      for (int j = 0; j < nrBdds; ++j)
		bdd_setbddpair(bitMap, j, genSort[j]);
	      leqRelation = bdd_veccompose(leqRelation, bitMap);
	    }
	  else
	    {
	      //
	      //	Free, sort constrained variable: sort assigned to free variable must be <= variables original sort.
	      //
	      int firstVar = realToBdd[i];
	      int nrBdds= sortBdds->getNrVariables(sort->component()->getIndexWithinModule());
	      for (int j = 0; j < nrBdds; ++j)
		bdd_setpair(bitMap, j, firstVar + j);
	      leqRelation = bdd_replace(leqRelation, bitMap);
	    }
	  bdd_freepair(bitMap);
	  unifier = bdd_and(unifier, leqRelation);
	  if (unifier == bddfalse)
	    return false;
	}
    }
  //
  //	Now compute a BDD that tells us if a unifier has maximal sort assignments to free
  //	variables.
  //
  //	maximal(X1,...,  Xn) = unifier(X1,..., Xn) &
  //	  for each i in 1 ... n
  //	    not(exists Yi .[gt(Yi,Xi) & unifier(X1,..., Y1,...., Xn)])
  //
  Bdd maximal = unifier;
  int secondBase = sortBdds->getFirstAvailableVariable();
  FOR_EACH_CONST(i, NatSet, sortConstrainedVariables)
    {
      int fv = *i;
      Sort* sort = variableIndexToSort(fv);
      int nrBddVariables = sortBdds->getNrVariables(sort->component()->getIndexWithinModule());
      //
      //	Replace the bdd variables allocated to the real variable in unifier with 0...nrBddVariables-1.
      //	Replace the bdd variables firstAvail,..., firstAvail+nrBddVariables-1 in gtOp with the
      //	variables allocated to real variable.
      //
      bddPair* realTofirstArg = bdd_newpair();
      bddPair* secondArgToReal = bdd_newpair();
      int firstVar = realToBdd[fv];
      for (int j = 0; j < nrBddVariables; ++j)
	{
	  int bddVariable = firstVar + j;
	  bdd_setpair(realTofirstArg, bddVariable, j);
	  bdd_setpair(secondArgToReal, secondBase + j, bddVariable);
	}
      Bdd gtRelation = sortBdds->getGtRelation(sort->component()->getIndexWithinModule());
      Bdd modifiedGtRelation = bdd_replace(gtRelation, secondArgToReal);
      bdd_freepair(secondArgToReal);
      Bdd modifiedUnifier = bdd_replace(unifier, realTofirstArg);
      bdd_freepair(realTofirstArg);
      maximal = bdd_and(maximal, bdd_appall(modifiedGtRelation, modifiedUnifier, bddop_nand,
					    sortBdds->makeVariableBdd(0, nrBddVariables)));
      Assert(maximal != bddfalse, "maximal false even though unifier isn't");
    }
  orderSortedUnifiers = new AllSat(maximal, secondBase, nextBddVariable - 1);
  return true;
}

bool
NarrowingUnificationProblem::extractUnifier()
{
  //
  //	We try to extract a unifier from the solved form by first ordering
  //	bound variables by their dependencies and if that can be done, by
  //	instantiating their bindings in that order.
  //
  order.clear();
  done.clear();
  pending.clear();
  freeVariables.clear();
  sortConstrainedVariables.clear();
  for (int i = 0; i < substitutionSize; ++i)
    {
      if (i < nrPreEquationVariables || i >= firstTargetSlot)
	{
	  if (unsortedSolution->value(i) == 0)
	    freeVariables.insert(i);
	  else if (!explore(i))
	    return false;
	}
    }
  FOR_EACH_CONST(i, Vector<int>, order)
    {
      //cout << "processing " << *i << endl;
      if (DagNode* d = unsortedSolution->value(*i)->instantiate(*unsortedSolution))
	unsortedSolution->bind(*i, d);
    }
  return true;
}

bool
NarrowingUnificationProblem::explore(int index)
{
  //
  //	Depth-first exploration of the dependencies of the binding for the
  //	variable with a given index.
  //
  if (done.contains(index))
    return true;
  DagNode* d = unsortedSolution->value(index);
  if (d == 0)
    {
      freeVariables.insert(index);
      sortConstrainedVariables.insert(index);
      done.insert(index);
      return true;
    }
  NatSet occurs;
  d->insertVariables(occurs);
  if (!occurs.disjoint(pending))
    return false;  // dependency cycle
  occurs.subtract(done);
  if (!occurs.empty())
    {
      pending.insert(index);
      FOR_EACH_CONST(i, NatSet, occurs)
	{
	  if (!explore(*i))
	    return false;
	}
      pending.subtract(index);
    }
  order.append(index);
  done.insert(index);
  return true; 
}
