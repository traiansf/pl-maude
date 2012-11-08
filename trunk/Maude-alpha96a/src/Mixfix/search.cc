/*

    This file is part of the Maude 2 interpreter.

    Copyright 1997-2003 SRI International, Menlo Park, CA 94025, USA.

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
//	Code for search command.
//

void
Interpreter::printSearchTiming(const Timer& timer,  RewriteSequenceSearch* state)
{
  if (getFlag(SHOW_STATS))
    {
      cout << "states: " << state->getNrStates() << "  ";
      printStats(timer, *(state->getContext()), getFlag(SHOW_TIMING));
    }
}

void
Interpreter::search(const Vector<Token>& bubble, Int64 limit, Int64 depth, SearchKind searchKind)
{
  VisibleModule* fm = currentModule->getFlatModule();
  Term* initial;
  int searchType;
  Term* target;
  Vector<ConditionFragment*> condition;
  if (!(fm->parseSearchCommand(bubble, initial, searchType, target, condition)))
    return;
  if (searchKind != SEARCH && !condition.empty())
    {
      IssueWarning(*target << ": conditions are not currently supported for narrowing");
      return;
    }

  Pattern* pattern = new Pattern(target, false, condition);
  if (!(pattern->getUnboundVariables().empty()))
    {
      IssueWarning(*target << ": variable " <<
		   QUOTE(pattern->index2Variable(pattern->getUnboundVariables().min())) <<
		   " is used before it is bound in the condition of a search command.\n");
      initial->deepSelfDestruct();
      delete pattern;
      return;
    }
  DagNode* subjectDag = makeDag(initial);
  
  static const char* searchTypeSymbol[] = { "=>1", "=>+", "=>*", "=>!" };
  if (getFlag(SHOW_COMMAND))
    {
      static const char* searchKindName[] = { "search", "narrow", "xg-narrow" };

      UserLevelRewritingContext::beginCommand();
      cout << searchKindName[searchKind] << ' ';
      printModifiers(limit, depth);
      cout << subjectDag << ' ' << searchTypeSymbol[searchType] << ' ' << pattern->getLhs();
      if (!condition.empty())
	{
	  cout << " such that ";
	  MixfixModule::printCondition(cout, condition);	  
	}
      cout << " ." << endl;
      if (xmlBuffer != 0)
	xmlBuffer->generateSearch(subjectDag, pattern, searchTypeSymbol[searchType], limit, depth);
    }

  startUsingModule(fm);

#ifdef QUANTIFY_REWRITING
  quantify_start_recording_data();
#endif
  
  if (searchKind == SEARCH)
    {
      RewriteSequenceSearch* state =
	new RewriteSequenceSearch(new UserLevelRewritingContext(subjectDag),
				  static_cast<RewriteSequenceSearch::SearchType>(searchType),
				  pattern,
				  depth);
      Timer timer(getFlag(SHOW_TIMING));
      doSearching(timer, fm, state, 0, limit);
    }
  else
    {
      NarrowingSequenceSearch* state = new NarrowingSequenceSearch(new UserLevelRewritingContext(subjectDag),
								   static_cast<NarrowingSequenceSearch::SearchType>(searchType),  // HACK
								   pattern,
								   depth,
								   (searchKind == XG_NARROW ?
								    NarrowingSearchState::ALLOW_NONEXEC | NarrowingSearchState::SINGLE_POSITION :
								    NarrowingSearchState::ALLOW_NONEXEC),
								   new FreshVariableSource(fm));
      Timer timer(getFlag(SHOW_TIMING));
      doNarrowing(timer, fm, state, 0, limit);
    }
}

void
Interpreter::doNarrowing(Timer& timer,
			 VisibleModule* module,
			 NarrowingSequenceSearch* state,
			 int solutionCount,
			 int limit)
{
  const VariableInfo* variableInfo = state->getGoal();
  int i = 0;
  for (; i != limit; i++)
    {
      bool result = state->findNextMatch();
      if (UserLevelRewritingContext::aborted())
	break;  // HACK: Is this safe - shouldn't we destroy context?
      if (!result)
	{
	  cout << ((solutionCount == 0) ? "\nNo solution.\n" : "\nNo more solutions.\n");
	  //printSearchTiming(timer, state);
	  break;
	}

      ++solutionCount;
      cout << "\nSolution " << solutionCount << "\n";
      //printSearchTiming(timer, state);
      UserLevelRewritingContext::printSubstitution(*(state->getSubstitution()), *variableInfo);
      if (UserLevelRewritingContext::interrupted())
	break;
    }

#ifdef QUANTIFY_REWRITING
  quantify_stop_recording_data();
#endif

  clearContinueInfo();  // just in case debugger left info
  //state->getContext()->clearCount();
  //savedRewriteSequenceSearch = state;
  //savedSolutionCount = solutionCount;
  //savedModule = module;
  //if (i == limit)  // possible to continue
  //  continueFunc = &Interpreter::searchCont;
  UserLevelRewritingContext::clearDebug();
}

void
Interpreter::doSearching(Timer& timer,
			 VisibleModule* module,
			 RewriteSequenceSearch* state,
			 int solutionCount,
			 int limit)
{
  const VariableInfo* variableInfo = state->getGoal();
  int i = 0;
  for (; i != limit; i++)
    {
      bool result = state->findNextMatch();
      if (UserLevelRewritingContext::aborted())
	break;  // HACK: Is this safe - shouldn't we destroy context?
      if (!result)
	{
	  cout << ((solutionCount == 0) ? "\nNo solution.\n" : "\nNo more solutions.\n");
	  printSearchTiming(timer, state);
	  if (xmlBuffer != 0)
	    {
	      xmlBuffer->generateSearchResult(NONE,
					      state,
					      timer,
					      getFlag(SHOW_STATS),
					      getFlag(SHOW_TIMING),
					      getFlag(SHOW_BREAKDOWN));
	    }
	  break;
	}

      ++solutionCount;
      cout << "\nSolution " << solutionCount <<
	" (state " << state->getStateNr() << ")\n";
      printSearchTiming(timer, state);
      UserLevelRewritingContext::printSubstitution(*(state->getSubstitution()), *variableInfo);
      if (UserLevelRewritingContext::interrupted())
	break;
      if (xmlBuffer != 0)
	{
	  xmlBuffer->generateSearchResult(solutionCount,
					  state,
					  timer,
					  getFlag(SHOW_STATS),
					  getFlag(SHOW_TIMING),
					  getFlag(SHOW_BREAKDOWN));
	}
    }

#ifdef QUANTIFY_REWRITING
  quantify_stop_recording_data();
#endif

  clearContinueInfo();  // just in case debugger left info
  state->getContext()->clearCount();
  savedRewriteSequenceSearch = state;
  savedSolutionCount = solutionCount;
  savedModule = module;
  if (i == limit)  // possible to continue
    continueFunc = &Interpreter::searchCont;
  UserLevelRewritingContext::clearDebug();
}

void
Interpreter::searchCont(Int64 limit, bool /* debug */)
{
  RewriteSequenceSearch* state = savedRewriteSequenceSearch;
  VisibleModule* fm = savedModule;
  savedRewriteSequenceSearch = 0;
  savedModule = 0;
  continueFunc = 0;
  if (xmlBuffer != 0 && getFlag(SHOW_COMMAND))
    xmlBuffer->generateContinue("search", fm, limit);

#ifdef QUANTIFY_REWRITING
  quantify_start_recording_data();
#endif

  Timer timer(getFlag(SHOW_TIMING));
  doSearching(timer, fm, state, savedSolutionCount, limit);
}

void
Interpreter::showSearchPath(int stateNr)
{
  if (savedRewriteSequenceSearch == 0)
    {
      IssueWarning("no state graph.");
      return;
    }
  if (stateNr < 0 || stateNr >= savedRewriteSequenceSearch->getNrStates())
    {
      IssueWarning("bad state number.");
      return;
    }
  if (xmlBuffer != 0 && getFlag(SHOW_COMMAND))
    xmlBuffer->generateShowSearchPath(stateNr);
  Vector<int> steps;
  for (int i = stateNr; i != NONE; i = savedRewriteSequenceSearch->getStateParent(i))
    steps.append(i);

  for (int i = steps.length() - 1; i >= 0; i--)
    {
      int sn = steps[i];
      if (sn != 0)
	cout << "===[ " << savedRewriteSequenceSearch->getStateRule(sn) << " ]===>\n";
      DagNode* d = savedRewriteSequenceSearch->getStateDag(sn);
      cout << "state " << sn << ", " << d->getSort() << ": " << d << '\n';
    }
  if (xmlBuffer != 0)
    xmlBuffer->generateSearchPath(savedRewriteSequenceSearch, stateNr);
}

void
Interpreter::showSearchPathLabels(int stateNr)
{
  if (savedRewriteSequenceSearch == 0)
    {
      IssueWarning("no state graph.");
      return;
    }
  if (stateNr < 0 || stateNr >= savedRewriteSequenceSearch->getNrStates())
    {
      IssueWarning("bad state number.");
      return;
    }
  Vector<int> steps;
  for (int i = stateNr; i != NONE; i = savedRewriteSequenceSearch->getStateParent(i))
    steps.append(i);

  int i = steps.length() - 2;
  if (i < 0)
    cout << "Empty path.\n";
  else
    { 
      for (; i >= 0; i--)
	{
	  const Label& label = savedRewriteSequenceSearch->getStateRule(steps[i])->getLabel();
	  if (label.id() == NONE)
	    cout << "(unlabeled rule)\n";
	  else
	    cout << &label << '\n';
	}
    }
}

void
Interpreter::showSearchGraph()
{
  if (savedRewriteSequenceSearch == 0)
    {
      IssueWarning("no state graph.");
      return;
    }
  if (xmlBuffer != 0 && getFlag(SHOW_COMMAND))
    xmlBuffer->generateShowSearchGraph();
  int nrStates = savedRewriteSequenceSearch->getNrStates();
  for (int i = 0; i < nrStates; i++)
    {
      if (i > 0)
	cout << '\n';
      DagNode* d = savedRewriteSequenceSearch->getStateDag(i);
      cout << "state " << i << ", " << d->getSort() << ": " << d << '\n';
      const RewriteSequenceSearch::ArcMap& fwdArcs =
	savedRewriteSequenceSearch->getStateFwdArcs(i);
      int arcNr = 0;
      for (RewriteSequenceSearch::ArcMap::const_iterator j = fwdArcs.begin();
	   j != fwdArcs.end(); j++, arcNr++)
	{
	  cout << "arc " << arcNr << " ===> state " << (*j).first;
	  const set<Rule*>& r = (*j).second;
	  for (set<Rule*>::const_iterator k = r.begin(); k != r.end(); k++)
	    cout << " (" << *k << ')';
	  cout << '\n';
	}
    }
  if (xmlBuffer != 0)
    xmlBuffer->generateSearchGraph(savedRewriteSequenceSearch);
}

void
Interpreter::getVariants(const Vector<Token>& bubble, Int64 limit, bool debug)
{
  VisibleModule* fm = currentModule->getFlatModule();
  Term* initial;
  Vector<Term*> constraint;

  if (!(fm->parseGetVariantsCommand(bubble, initial, constraint)))
    return;

  DagNode* d = makeDag(initial);
       //  if (DagNode* d = makeDag(subject))
    {
      if (getFlag(SHOW_COMMAND))
	{
	  UserLevelRewritingContext::beginCommand();
	  cout << "get variants ";
	  if (limit != NONE)
	    cout  << '[' << limit << "] ";
	  cout << "in " << currentModule << " : " << d;
	  if (constraint.empty())
	    cout << " ." << endl;
	  else
	    {
	      cout << " such that ";
	      const char* sep = "";
	      FOR_EACH_CONST(i, Vector<Term*>, constraint)
		{
		  cout << sep << *i;
		  sep = ", ";
		}
	      cout << " irreducible ." << endl;
	    }
	}
      VisibleModule* fm = currentModule->getFlatModule();
      startUsingModule(fm);

      Timer timer(getFlag(SHOW_TIMING));

      FreshVariableGenerator* freshVariableGenerator = new FreshVariableSource(fm);
      UserLevelRewritingContext* context = new UserLevelRewritingContext(d);
      if (debug)
	UserLevelRewritingContext::setDebug();

      Vector<DagNode*> blockerDags;
      FOR_EACH_CONST(i, Vector<Term*>, constraint)
	{
	  Term* t = *i;
	  t = t->normalize(true);  // we don't really need to normalize but we do need to set hash values
	  blockerDags.append(t->term2Dag());
	  t->deepSelfDestruct();
	}
      //
      //	Responsibility for deleting context and freshVariableGenerator is passed to ~VariantSearch().
      //
      VariantSearch vs(context, blockerDags, freshVariableGenerator);

      if (!(context->traceAbort()))
	{
	  const NarrowingVariableInfo& variableInfo = vs.getVariableInfo();
	  printStats(timer, *context, getFlag(SHOW_TIMING));
	  cout << endl;

	  int counter = 0;
	  const Vector<DagNode*>* variant;
	  int nrFreeVariables;
	  while (counter != limit && (variant = vs.getNextVariant(nrFreeVariables)))
	    {
	      ++counter;
	      cout << "Variant #" << counter << endl;
	      int nrVariables = variant->size() - 1;
	      DagNode* d = (*variant)[nrVariables];
	      cout << d->getSort() << ": " << d << '\n';
	      for (int i = 0; i < nrVariables; ++i)
		{
		  DagNode* v = variableInfo.index2Variable(i);
		  cout << v << " --> " << (*variant)[i] << endl;
		}
	      cout << endl;
	    }
	  if (counter == 0)
	    cout << "No variants.\n";
	}
      (void) fm->unprotect();
      UserLevelRewritingContext::clearDebug();
    }
}

void
Interpreter::variantUnify(const Vector<Token>& bubble, Int64 limit, bool debug)
{
  VisibleModule* fm = currentModule->getFlatModule();
  Vector<Term*> lhs;
  Vector<Term*> rhs;
  Vector<Term*> constraint;

  if (!(fm->parseVariantUnifyCommand(bubble, lhs, rhs, constraint)))
    return;

  if (getFlag(SHOW_COMMAND))
    {
      UserLevelRewritingContext::beginCommand();
      cout << "variant unify ";
      if (limit != NONE)
	cout << '[' << limit << "] ";
      cout << "in " << currentModule << " : ";
      int nrPairs = lhs.size();
      for (int i = 0; i < nrPairs; ++i)
	cout << lhs[i] << " =? " << rhs[i] << ((i == nrPairs - 1) ? " " : " /\\ ");
      if (constraint.empty())
	cout << " ." << endl;
      else
	{
	  cout << " such that ";
	  const char* sep = "";
	  FOR_EACH_CONST(i, Vector<Term*>, constraint)
	    {
	      cout << sep << *i;
	      sep = ", ";
	    }
	  cout << " irreducible ." << endl;
	}
    }

  startUsingModule(fm);
  Timer timer(getFlag(SHOW_TIMING));
  FreshVariableGenerator* freshVariableGenerator = new FreshVariableSource(fm);
  if (debug)
    UserLevelRewritingContext::setDebug();

  DagNode* d = fm->makeUnificationProblemDag(lhs, rhs);
  UserLevelRewritingContext* context = new UserLevelRewritingContext(d);

  Vector<DagNode*> blockerDags;
  FOR_EACH_CONST(i, Vector<Term*>, constraint)
    {
      Term* t = *i;
      t = t->normalize(true);  // we don't really need to normalize but we do need to set hash values
      blockerDags.append(t->term2Dag());
      t->deepSelfDestruct();
    }

  //
  //	Responsibility for deleting context and freshVariableGenerator is passed to ~VariantSearch().
  //
  VariantSearch vs(context, blockerDags, freshVariableGenerator, true);

  if (!(context->traceAbort()))
    {
      const NarrowingVariableInfo& variableInfo = vs.getVariableInfo();
      printStats(timer, *context, getFlag(SHOW_TIMING));
      cout << endl;

      int counter = 0;
      const Vector<DagNode*>* unifier;
      int nrFreeVariables;
      while (counter != limit && (unifier = vs.getNextUnifier(nrFreeVariables)))
	{
	  ++counter;
	  cout << "Unifier #" << counter << endl;
	  int nrVariables = unifier->size();

	  for (int i = 0; i < nrVariables; ++i)
	    {
	      DagNode* v = variableInfo.index2Variable(i);
	      cout << v << " --> " << (*unifier)[i] << endl;
	    }
	  cout << endl;
	}
      if (counter == 0)
	cout << "No unifiers.\n";
    }
  (void) fm->unprotect();
  UserLevelRewritingContext::clearDebug();
}
