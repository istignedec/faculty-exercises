
"""
In search.py, you will implement generic search algorithms which are called by
Pacman agents (in searchAgents.py).
"""

import util
from logic import * 

class SearchProblem:
    """
    This class outlines the structure of a search problem, but doesn't implement
    any of the methods (in object-oriented terminology: an abstract class).

    You do not need to change anything in this class, ever.
    """

    def getStartState(self):
        """
        Returns the start state for the search problem.
        """
        util.raiseNotDefined()

    def isGoalState(self, state):
        """
          state: Search state

        Returns True if and only if the state is a valid goal state.
        """
        util.raiseNotDefined()

    def getSuccessors(self, state):
        """
        state: Search state

        For a given state, this should return a list of triples, (successor,
        action, stepCost), where 'successor' is a successor to the current
        state, 'action' is the action required to get there, and 'stepCost' is
        the incremental cost of expanding to that successor.
        """
        util.raiseNotDefined()

    def getCostOfActions(self, actions):
        """
        actions: A list of actions to take

        This method returns the total cost of a particular sequence of actions.
        The sequence must be composed of legal moves.
        """
        util.raiseNotDefined()


def miniWumpusSearch(problem): 
    """
    A sample pass through the miniWumpus layout. Your solution will not contain 
    just three steps! Optimality is not the concern here.
    """
    from game import Directions
    e = Directions.EAST 
    n = Directions.NORTH
    return  [e, n, n]

def logicBasedSearch(problem):
    """

    To get started, you might want to try some of these simple commands to
    understand the search problem that is being passed in:

    print "Start:", problem.getStartState()
    print "Is the start a goal?", problem.isGoalState(problem.getStartState())
    print "Start's successors:", problem.getSuccessors(problem.getStartState())

    print "Does the Wumpus's stench reach my spot?", 
               \ problem.isWumpusClose(problem.getStartState())

    print "Can I sense the chemicals from the pills?", 
               \ problem.isPoisonCapsuleClose(problem.getStartState())

    print "Can I see the glow from the teleporter?", 
               \ problem.isTeleporterClose(problem.getStartState())
    
    (the slash '\\' is used to combine commands spanning through multiple lines - 
    you should remove it if you convert the commands to a single line)
    
    Feel free to create and use as many helper functions as you want.

    A couple of hints: 
        * Use the getSuccessors method, not only when you are looking for states 
        you can transition into. In case you want to resolve if a poisoned pill is 
        at a certain state, it might be easy to check if you can sense the chemicals 
        on all cells surrounding the state. 
        * Memorize information, often and thoroughly. Dictionaries are your friends and 
        states (tuples) can be used as keys.
        * Keep track of the states you visit in order. You do NOT need to remember the
        tranisitions - simply pass the visited states to the 'reconstructPath' method 
        in the search problem. Check logicAgents.py and search.py for implementation.
    """
    # array in order to keep the ordering
    visitedStates = []
    startState = problem.getStartState()
    visitedStates.append(startState)
    """
    ####################################
    ###                              ###
    ###        YOUR CODE HERE        ###
    ###                              ###
    ####################################
    """

    knowledgeBase = dict() # baza znanja za otkrivene informacije
    safe = set() # lista sigurnih stanja
    safeStates = set() # pomocno
    dangerous = set() # lista opasnih (nesigurnih) stanja
    undecided = set() # niti sigurna, niti nesigurna
    state = startState
    wumpusLocation = None
    poisonLocation = None

    while(True):
        print "\nVisiting: ", state
        if(problem.isGoalState(state)):
            print "Game over: Teleported home!"
            return problem.reconstructPath(visitedStates)
        elif(problem.isWumpus(state) or problem.isPoisonCapsule(state)):
            print "Game over: DEAD!"
            return problem.reconstructPath(visitedStates)
        
        # racunaj za svako stanje (polje)
        S = problem.isWumpusClose(state) # smell
        C = problem.isPoisonCapsuleClose(state) # chemicals
        G = problem.isTeleporterClose(state) # goal
        knowledgeBase.setdefault(state, {}).update({'S': S, 'C': C, 'G': G, 'O': True})
        
        print "Sensed: ", S, "S", state
        print "Sensed: ", C, "C", state
        print "Sensed: ", G, "G", state
        succStates = [s[0] for s in problem.getSuccessors(state)] # dohvati sljedeca stanja i obradi ih
        nextStateFunction(state, succStates, visitedStates, dangerous, undecided, safe, knowledgeBase, wumpusLocation, safeStates)

        if safeStates: # ako ima sigurnih stanja, dohvati sljedece sigurno po prioritetu
            state = minStateWeight(list(safeStates))
            safeStates.remove(state)
        elif undecided: # ako nema sigurnih, a ima nesigurnih (neodredjenih)
            found = False
            unsure = set()
            st = None

            while undecided and not found: # dok ne nadjemo O
                st = minStateWeight(list(undecided)) # dohvati stanje najmanje tezine iz nesigurnih
                undecided.remove(st)

                W = knowledgeBase.setdefault(st, {}).get('W', None)
                P = knowledgeBase.setdefault(st, {}).get('P', None)
                T = knowledgeBase.setdefault(st, {}).get('T', None)
                O = knowledgeBase.setdefault(st, {}).get('O', None)

                if W or P:
                    continue
                if O: # nasli smo sigurno
                    found = True
                elif not W and not P:
                    if (W is not None) and (P is not None):
                        found = True
                    else:
                        unsure.add(st)

            undecided = undecided | unsure

            if found:
                state = st
            else:
                print "Game over: No more moving!"
                return problem.reconstructPath(visitedStates)
        
        else:
            print "Game over: No more moving!"
            return problem.reconstructPath(visitedStates)
        
        visitedStates.append(state)

def nextStateFunction(state, succStates, visitedStates, dangerous, undecided, safe, knowledgeBase, wumpusLocation, safeStates):
    for nextS in succStates: # za svako slijedece stanje
        if (nextS not in visitedStates) and (nextS not in dangerous): # preskoci opasna i posjecena stanja
            knowledgeOfState = knowledgeBase.setdefault(nextS, {})
            
            clauses = getClauses(succStates, state, knowledgeBase, wumpusLocation) # racun logike

            W = checkConclusion(knowledgeOfState, clauses, nextS, 'W')
            P = checkConclusion(knowledgeOfState, clauses, nextS, 'P')
            T = checkConclusion(knowledgeOfState, clauses, nextS, 'T')
            O = checkConclusion(knowledgeOfState, clauses, nextS, 'O')

            knowledgeOfState.update({'W': W, 'P': P, 'T': T, 'O': O})
            if W != None:
                print "Concluded: ", W, "W", nextS
            if P != None:
                print "Concluded: ", P, "P", nextS
            if T != None:
                print "Concluded: ", T, "T", nextS
            if O != None:
                print "Concluded: ", O, "O", nextS

            if T: # ako je teleporter
                state = nextS
                visitedStates.append(state)
                return problem.reconstructPath(visitedStates)
            elif O: # ako je sigurno polje, O
                if nextS not in safeStates:
                    if nextS in undecided:
                        undecided.remove(nextS)
                    safeStates.add(nextS)
                    safe.add(nextS)
            elif W:
                if nextS in undecided:
                    undecided.remove(nextS)
                dangerous.add(nextS)
                wumpusLocation = nextS
            elif P:
                if nextS in undecided:
                    undecided.remove(nextS)
                dangerous.add(nextS)
                poisonLocation = nextS
            else:
                undecided.add(nextS)

def checkConclusion(knowledgeOfState, clauses, state, label):
    result = knowledgeOfState.get(label, None)
    if result is not None:
        return result
    else:
        result = resolution(clauses, Clause(Literal(label, state, False))) # primjena rezolucije opovrgavanjem
        if result:
            return result

def minStateWeight(states): # nadji stanje sa najvecom prednosti
    minState = states[0]
    min = stateWeight(minState) # najmanja tezina stanja
    for state in states:
        if stateWeight(state) < min:
            min = stateWeight(state)
            minState = state
    return minState

def getClauses(nextS, state, knowledgeBase, wumpusLocation):
    S = knowledgeBase.setdefault(state, {}).get('S', None)
    C = knowledgeBase.setdefault(state, {}).get('C', None)
    G = knowledgeBase.setdefault(state, {}).get('G', None)

    WClauses = getWClauses(nextS, knowledgeBase, wumpusLocation, S)
    PClauses = getPClauses(nextS, knowledgeBase, C)
    TClauses = getTClauses(nextS, knowledgeBase, G)
    OClauses = getOClauses(nextS, knowledgeBase, S, C)

    clauses = WClauses | PClauses | TClauses | OClauses
    return clauses

def getWClauses(nextS, knowledgeBase, wumpusLocation, S):
    WClauses = set()
    if S:
        literals = set()
        for s in nextS:
            W = knowledgeBase.setdefault(s, {}).get('W', None)
            if W is not None:
                WClauses.add(Clause({Literal('W', s, not W)}))
            else:
                if wumpusLocation is not None:
                    WClauses.add(Clause({Literal('W', s, not (s == wumpusLocation))}))
                else:
                    literals.add(Literal('W', s, False))

        WClauses.add(Clause(literals))
    else:
        for s in nextS:
            WClauses.add(Clause({Literal('W', s, True)}))
            knowledgeBase.setdefault(s, {}).update({'W': False})
    return WClauses

def getPClauses(nextS, knowledgeBase, C):
    PClauses = set()
    if C:
        literals = set()
        for s in nextS:
            P = knowledgeBase.setdefault(s, {}).get('P', None)
            if P is not None:
                PClauses.add(Clause({Literal('P', s, not P)}))
            else:
                literals.add(Literal('P', s, False))
        PClauses.add(Clause(literals))
    else:
        for s in nextS:
            PClauses.add(Clause({Literal('P', s, True)}))
            knowledgeBase.setdefault(s, {}).update({'P': False})
    return PClauses

def getTClauses(nextS, knowledgeBase, G):
    TClauses = set()
    if G:
        literals = set()
        for s in nextS:
            literals.add(Literal('T', s, False))
        TClauses.add(Clause(literals))
    else:
        for s in nextS:
            TClauses.add(Clause({Literal('T', s, True)}))
            knowledgeBase.setdefault(s, {}).update({'T': False})
    return TClauses

def getOClauses(nextS, knowledgeBase, S, C):
    OClauses = set()
    if not C and not S:
        for s in nextS:
            OClauses.add(Clause({Literal('O', s, False)}))
            knowledgeBase.setdefault(s, {}).update({'O': True})

    for s in nextS:
        W = knowledgeBase.setdefault(s, {}).get('W', None)
        P = knowledgeBase.setdefault(s, {}).get('P', None)

        if W or P:
            OClauses.add(Clause({Literal('O', s, True)}))
            knowledgeBase.setdefault(s, {}).update({'O': False})
        elif (W is not None) and (P is not None) and (not W) and (not P):
            OClauses.add(Clause({Literal('O', s, False)}))
            knowledgeBase.setdefault(s, {}).update({'O': True})
    return OClauses

####################################
###                              ###
###        YOUR CODE THERE       ###
###                              ###
####################################

"""
        ####################################
        ###                              ###
        ###      YOUR CODE EVERYWHERE    ###
        ###                              ###
        ####################################
"""

# Abbreviations
lbs = logicBasedSearch
