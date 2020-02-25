import util 
import functools 
import copy

class Labels:
    """
    Labels describing the WumpusWorld
    """
    WUMPUS = 'w'
    TELEPORTER = 't'
    POISON = 'p'
    SAFE = 'o'

    """
    Some sets for simpler checks
    >>> if literal.label in Labels.DEADLY: 
    >>>     # Don't go there!!!
    """ 
    DEADLY = set([WUMPUS, POISON])
    WTP = set([WUMPUS, POISON, TELEPORTER])

    UNIQUE = set([WUMPUS, POISON, TELEPORTER, SAFE])

    POISON_FUMES = 'b'
    TELEPORTER_GLOW = 'g'
    WUMPUS_STENCH = 's'

    INDICATORS = set([POISON_FUMES, TELEPORTER_GLOW, WUMPUS_STENCH])


def stateWeight(state):
    """
    To ensure consistency in exploring states, they will be sorted 
    according to a simple linear combination. 
    The maps will never be 
    larger than 20x20, and therefore this weighting will be consistent.
    """
    x, y = state 
    return 20*x + y 


@functools.total_ordering
class Literal:
    """
    A literal is an atom or its negation
    In this case, a literal represents if a certain state (x,y) is or is not 
    the location of GhostWumpus, or the poisoned pills.
    """

    def __init__(self, label, state, negative=False):
        """
        Set all values. Notice that the state is remembered twice - you
        can use whichever representation suits you better.
        """
        x,y = state 
        
        self.x = x 
        self.y = y 
        self.state = state 

        self.negative = negative
        self.label = label 

    def __key(self):
        """
        Return a unique key representing the literal at a given point
        """
        return (self.x, self.y, self.negative, self.label)

    def __hash__(self):
        """
        Return the hash value - this operator overloads the hash(object) function.
        """
        return hash(self.__key())

    def __eq__(first, second):
        """
        Check for equality - this operator overloads '=='
        """
        return first.__key() == second.__key()

    def __lt__(self, other):
        """ 
        Less than check
        by using @functools decorator, this is enough to infer ordering
        """
        return stateWeight(self.state) < stateWeight(other.state)

    def __str__(self):
        """
        Overloading the str() operator - convert the object to a string
        """
        if self.negative: return '~' + self.label
        return self.label

    def __repr__(self):
        """
        Object representation, in this case a string
        """
        return self.__str__()

    def copy(self):
        """
        Return a copy of the current literal
        """
        return Literal(self.label, self.state, self.negative)

    def negate(self):
        """
        Return a new Literal containing the negation of the current one
        """
        return Literal(self.label, self.state, not self.negative)

    def isDeadly(self):
        """
        Check if a literal represents a deadly state
        """
        return self.label in Labels.DEADLY

    def isWTP(self):
        """
        Check if a literal represents GhostWumpus, the Teleporter or 
        a poisoned pill
        """
        return self.label in Labels.WTP

    def isSafe(self):
        """
        Check if a literal represents a safe spot
        """
        return self.label == Labels.SAFE

    def isTeleporter(self):
        """
        Check if a literal represents the teleporter
        """
        return self.label == Labels.TELEPORTER


class Clause: 
    """ 
    A disjunction of finitely many unique literals. 
    The Clauses have to be in the CNF so that resolution can be applied to them. The code 
    was written assuming that the clauses are in CNF, and will not work otherwise. 

    A sample of instantiating a clause (~B v C): 

    >>> premise = Clause(set([Literal('b', (0, 0), True), Literal('c', (0, 0), False)]))

    or; written more clearly
    >>> LiteralNotB = Literal('b', (0, 0), True)
    >>> LiteralC = Literal('c', (0, 0), False)

    >>> premise = Clause(set([[LiteralNotB, LiteralC]]))
    """ 

    def __init__(self, literals):
        """
        The constructor for a clause. The clause assumes that the data passed 
        is an iterable (e.g., list, set), or a single literal in case of a unit clause. 
        In case of unit clauses, the Literal is wrapped in a list to be safely passed to 
        the set.
        """
        if not type(literals) == set and not type(literals) == list:
            self.literals = set([literals])
        else:
            self.literals = set(literals)

    def isResolveableWith(self, otherClause):
        """
        Check if a literal from the clause is resolveable by another clause - 
        if the other clause contains a negation of one of the literals.
        e.g., (~A) and (A v ~B) are examples of two clauses containing opposite literals 
        """
        for literal in self.literals: 
            if literal.negate() in otherClause.literals:
                return True 
        return False 

    def isRedundant(self, otherClauses):
        """
        Check if a clause is a subset of another clause.
        """
        for clause in otherClauses:
            if self == clause: continue
            if clause.literals.issubset(self.literals):
                return True
        return False

    def negateAll(self):
        """
        Negate all the literals in the clause to be used 
        as the supporting set for resolution.
        """
        negations = set()
        for literal in self.literals:
            clause = Clause(literal.negate())
            negations.add(clause)
        return negations

    def __str__(self):
        """
        Overloading the str() operator - convert the object to a string
        """
        return ' V '.join([str(literal) for literal in self.literals])

    def __repr__(self):
        """
        The representation of the object
        """
        return self.__str__()

    def __key(self):
        """
        Return a unique key representing the literal at a given point
        """
        return tuple(sorted(list(self.literals)))

    def __hash__(self):
        """
        Return the hash value - this operator overloads the hash(object) function.
        """
        return hash(self.__key())

    def __eq__(first, second):
        """
        Check for equality - this operator overloads '=='
        """
        return first.__key() == second.__key()


def resolution(clauses, goal):
    """
    Implement refutation resolution. 

    The pseudocode for the algorithm of refutation resolution can be found 
    in the slides. The implementation here assumes you will use set of support 
    and simplification strategies. We urge you to go through the slides and 
    carefully design the code before implementing.
    """
    resolvedPairs = set()
    setOfSupport = goal.negateAll() 
    """
    ####################################
    ###                              ###
    ###        YOUR CODE HERE        ###
    ###                              ###
    ####################################
    """
    while(True):
        removeRedundant(clauses, setOfSupport); # ukloni redundantne klauzule

        # uklanjanje nevaznih klauzula (valjanje, tautologije)
        removed = set()
        all = clauses | setOfSupport
        for clause in all:
            if clause.isResolveableWith(clause):
                for literal in clause.literals:
                    if literal.negate() in clause.literals:
                        removedClauses.add(clauses)

        for clause in removed:
            if clause in setOfSupport:
                setOfSupport.remove(clause)
            else:
                clauses.remove(clause)
        
        pairs = selectClauses(clauses, setOfSupport, resolvedPairs)
        if not pairs:
            return False
        
        new = set()
        for pair in pairs:
            c1, c2 = pair
            resolvents = resolvePair(c1, c2)
            resolvedPairs.add(pair)

            # provjera postojanja NIL
            if not resolvents.literals:
                return True

            new.add(resolvents)

        setOfSupport = setOfSupport | new


def removeRedundant(clauses, setOfSupport):
    """
    Remove redundant clauses (clauses that are subsets of other clauses)
    from the aforementioned sets. 
    Be careful not to do the operation in-place as you will modify the 
    original sets. (why?)
    ####################################
    ###                              ###
    ###        YOUR CODE HERE        ###
    ###                              ###
    ####################################
    """
    removed = set() # bez ponavljanja
    all = clauses | setOfSupport # dodaj klauzule dobivene negacijom cilja
    for clause in all:
        if clause.isRedundant(all):
            removed.add(clause) # ako se ponavlja, dodaj u listu uklonjenih

    for clause in removed: # stvarno uklanjanje
        if clause in setOfSupport:
            setOfSupport.remove(clause)
        else:
            clauses.remove(clause)

def resolvePair(firstClause, secondClause):
    """
    Resolve a pair of clauses.
    ####################################
    ###                              ###
    ###        YOUR CODE HERE        ###
    ###                              ###
    ####################################
    """
    newClause = set() # nova klauzula je set, nema ponavljanja
    secClause = copy.deepcopy(secondClause.literals) # deepcopy literala da ih ne mijenja unutar secondClause
    for literal in firstClause.literals:
        if literal.negate() in secondClause.literals:
            secClause.remove(literal.negate()) # ako postoji negacija u prvoj, ukloni ju u drugoj
        else:
            newClause.add(literal) # ako ne postoji, samo dodaj, disjunkcija

    newClause = newClause | secClause # dodaj u novu klauzulu ove iz druge (iz koje su izbaceni +/- parovi)
    return Clause(newClause)

def selectClauses(clauses, setOfSupport, resolvedPairs):
    """
    Select pairs of clauses to resolve.
    ####################################
    ###                              ###
    ###        YOUR CODE HERE        ###
    ###                              ###
    ####################################
    """
    # odaberi skup parova klauzula za rjesavanje
    selected = set() # neporedano, bez duplikata
    # kombiniramo klauzule ulaznih premisa s klauzulama SoS
    all = clauses | setOfSupport # setOfSupport su klauzule dobivene negacijom cilja
    for sos in setOfSupport:
        for clause in all:
            if clause != sos and clause.isResolveableWith(sos):
                pair = clause, sos # jedan clan para iz SoS, drugi iz Sos U clauses
                if pair not in resolvedPairs:
                    selected.add(pair)

    return selected

def testResolution():
    """
    A sample of a resolution problem that should return True. 
    You should come up with your own tests in order to validate your code. 
    """
    premise1 = Clause(set([Literal('a', (0, 0), True), Literal('b', (0, 0), False)])) # (~A v B)
    premise2 = Clause(set([Literal('b', (0, 0), True), Literal('c', (0, 0), False)])) # (~B v C)
    premise3 = Clause(Literal('a', (0,0))) # A
    goal = Clause(Literal('c', (0,0))) # C
    print "TEST 1"
    print resolution(set([premise1, premise2, premise3]), goal)

    premise1 = Clause(set([Literal('a', (0, 0), False), Literal('b', (0, 0), True)])) # (A v ~B)
    premise2 = Clause(set([Literal('b', (0, 0), False), Literal('c', (0, 0), False)])) # (B v C)
    premise3 = Clause(set([Literal('a', (0, 0), True), Literal('c', (0, 0), True)])) # (~A v ~C)
    goal = Clause(set([Literal('b', (0, 0), True), Literal('c', (0, 0), True)])) # (~B v ~C)
    print "TEST 2"
    print resolution(set([premise1, premise2, premise3]), goal) # treba biti True

    premise1 = Clause(set([Literal('a', (0, 0), True), Literal('a', (0, 0), True)])) # (~A v ~A)
    premise2 = Clause(set([Literal('a', (0, 0), False), Literal('a', (0, 0), False)])) # (A v A)
    goal = Clause(set([Literal('a', (0, 0), False), Literal('a', (0, 0), True)])) # (A v ~A)
    print "TEST 3"
    print resolution(set([premise1, premise2]), goal) # treba biti True

    premise1 = Clause(set([Literal('a', (0, 0), False), Literal('b', (0, 0), False)])) # (A v B)
    premise2 = Clause(set([Literal('b', (0, 0), False), Literal('c', (0, 0), False)])) # (B v C)
    premise3 = Clause(set([Literal('a', (0, 0), True), Literal('c', (0, 0), True)])) # (~A v ~C)
    goal = Clause(set([Literal('b', (0, 0), True), Literal('c', (0, 0), True)])) # (~B v ~C)
    print "TEST 4"
    print resolution(set([premise1, premise2, premise3]), goal) # treba biti False

    premise = Clause(Literal('a', (0,0))) # A
    goal = Clause(set([Literal('a', (0, 0), False), Literal('a', (0, 0), False)])) # (A v B)
    print "TEST 5"
    print resolution(set([premise]), goal) # treba biti True

if __name__ == '__main__':
    """
    The main function - if you run logic.py from the command line by 
    >>> python logic.py 

    this is the starting point of the code which will run. 
    """ 
    testResolution() 