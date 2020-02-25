import util
import math

class NaiveBayesClassifier(object):
    """
    See the project description for the specifications of the Naive Bayes classifier.

    Note that the variable 'datum' in this code refers to a counter of features
    (not to a raw samples.Datum).
    """
    def __init__(self, legalLabels, smoothing=0, logTransform=False, featureValues=util.Counter()):
        self.legalLabels = legalLabels
        self.type = "naivebayes"
        self.k = int(smoothing) # this is the smoothing parameter, ** use it in your train method **
        self.logTransform = logTransform
        self.featureValues = featureValues # empty if there is no smoothing

    def fit(self, trainingData, trainingLabels):
        """
        Trains the classifier by collecting counts over the training data, and
        stores the Laplace smoothed estimates so that they can be used to classify.
        
        trainingData is a list of feature dictionaries.  The corresponding
        label lists contain the correct label for each instance.

        To get the list of all possible features or labels, use self.features and self.legalLabels.
        """

        self.features = trainingData[0].keys() # the names of the features in the dataset

        self.prior = util.Counter() # probability over labels >>> P(hi)
        self.conditionalProb = util.Counter() # Conditional probability of feature feat for a given class having value v
                                      # HINT: could be indexed by (feat, label, value)
                                    # >>> P(fj|hi)

        # TODO:
        # construct (and store) the normalized smoothed priors and conditional probabilities

        "*** YOUR CODE HERE ***"

        values = set()
        for i in range(len(trainingData)): # iteracija po znacajkama
            instance = trainingData[i] # instanca je konkretna situacija (jedan rjecnik)
            label = trainingLabels[i] # npr. east
            self.prior[label] += 1.0 # broji koliko ima east, west...

            for feature in self.features:
                value = instance[feature] # za svaki feature izvadi vrijednost (true, false)
                values.add(value)
                self.conditionalProb[(feature, label, value)] += 1.0 # broji koliko ima triplea

        for feature in self.features: # za svaki feature
            for label in self.legalLabels: # za svaki label (east, west...)
                labelNumber = self.prior[label] # broj pojave east

                sum = 0
                for value in values: # za svaki value (true, false)
                    key = feature, label, value
                    self.conditionalProb[key] = (self.conditionalProb[key] + self.k) / float(labelNumber + self.k * len(self.features)) # (X + k) / (L + k * D)
                    sum += self.conditionalProb[key]

                for value in values:
                    key = feature, label, value
                    self.conditionalProb[key] /= float(sum) # "objedini" true i false

        for label in self.prior.keys():
            self.prior[label] = (self.prior[label] + self.k) / float(len(trainingLabels) + self.k * len(self.prior.keys())) # isto kao gore, zagladjivanje


    def predict(self, testData):
        """
        Classify the data based on the posterior distribution over labels.

        You shouldn't modify this method.
        """

        guesses = []
        self.posteriors = [] # posterior probabilities are stored for later data analysis.
        
        for instance in testData:
            if self.logTransform:
                posterior = self.calculateLogJointProbabilities(instance)
            else:
                posterior = self.calculateJointProbabilities(instance)
            guesses.append(posterior.argMax())
            self.posteriors.append(posterior)
        return guesses


    def calculateJointProbabilities(self, instance):
        """
        Returns the joint distribution over legal labels and the instance.
        Each probability should be stored in the joint counter, e.g.
        Joint[3] = <Estimate of ( P(Label = 3, instance) )>

        To get the list of all possible features or labels, use self.features and
        self.legalLabels.
        """
        joint = util.Counter()

        for label in self.legalLabels:
            # calculate the joint probabilities for each class
            "*** YOUR CODE HERE ***"
            posterior = 1
            for feature in self.features:
                value = instance[feature]
                posterior *= self.conditionalProb[(feature, label, value)] # umnozak P(fj|hi)
            joint[label] = self.prior[label] * posterior # jos dodati P(hi)

        return joint


    def calculateLogJointProbabilities(self, instance):
        """
        Returns the log-joint distribution over legal labels and the instance.
        Each log-probability should be stored in the log-joint counter, e.g.
        logJoint[3] = <Estimate of log( P(Label = 3, instance) )>

        To get the list of all possible features or labels, use self.features and
        self.legalLabels.
        """
        logJoint = util.Counter()

        for label in self.legalLabels:
            #calculate the log joint probabilities for each class
            "*** YOUR CODE HERE ***"
            posterior = 0
            for feature in self.features:
                value = instance[feature]
                posterior += math.log(self.conditionalProb[(feature, label, value)]) # umnozak P(fj|hi)
            logJoint[label] = math.log(self.prior[label]) + posterior # jos dodati P(hi)

        return logJoint # koristi se za male brojeve
