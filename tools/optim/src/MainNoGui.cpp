#include <argp.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <embryo.h>
#include <ArrayOps.h>
#include "cmaes/cmaes_interface.h"
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <omp.h>
#include <set>
#include "Picture.h"

using namespace std;
using namespace cmaes;
using namespace embryo;

int gWidth = 512;
int gHeight = 512;
bool bIsGuiOutputWindow;

class exceptionPics : public std::exception {
public:

    virtual const char* what() const throw () {
        return "more than one pic on input";
    }
};

class CmdLineConf {
    long mRandomSeed;
    std::string mConfigFileName;
    std::string mTestContentPath;

public:

    CmdLineConf() {
        mRandomSeed = 0;
    }

    inline const std::string& configFileName() const {
        return mConfigFileName;
    }

    inline std::string& configFileName() {
        return mConfigFileName;
    }

    inline std::string& testContentPath() {
        return mTestContentPath;
    }

    inline long seed() const {
        return mRandomSeed;
    }

    inline long& seed() {
        return mRandomSeed;
    }
}
; // struct CmdLineConf


const char* argp_program_version = "embryo-optim";

const char *argp_program_bug_address = "<janahlava@gmail.com>";

static char doc[] = "embryo-optim -- CMAES optimizer for cellular embryogenetic flags";

static struct argp_option options[]
        = {
    {
        "seed", 's', "SEED", 0,
        "Random seed value"
    },
    {
        "path", 'p', "PATH", 0,
        "test content path with embryo configuration file and pictures files"
    },
    {
        "config", 'c', "CONFIG", 0,
        "CMAES configuration file"
    },
    //    {
    //        "init", 'i', "INIT", 0,
    //        "different initial values of states/chemicals depending on a distance from the image border"
    //    },
    {
        0
    }
};

static error_t
parseCmdLineOptions(int key, char* arg, struct argp_state* state) {
    CmdLineConf* lCmdLineConf = (CmdLineConf*) state->input;

    switch (key) {
        case 'c':
            lCmdLineConf->configFileName() = arg;
            break;

        case 'p':
            lCmdLineConf->testContentPath() = arg;
            break;
	    
        case 's':
        {
            long lValue;
            istringstream lBuffer(arg);
            lBuffer >> lValue;
            lCmdLineConf->seed() = lValue;
        }
            break;

        case ARGP_KEY_ARG:
            break;

        case ARGP_KEY_END:
            break;

        default:
            return ARGP_ERR_UNKNOWN;
    }

    return 0;
}



static struct argp argp
        = {
    options, parseCmdLineOptions, 0, doc
};

int
safeOpen(ifstream& inFile, const std::string& inFileName) {
    inFile.open(inFileName.c_str());
    if (!inFile.good()) {
        cerr
                << "Unable to open '"
                << inFileName
                << "'"
                << endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

std::string
buildWindowTitle(size_t inNbFitness, double inCurrentFitness) {
    ostringstream lBuffer;

    lBuffer << argp_program_version << " | ";

    lBuffer << inNbFitness << " pop ";

    lBuffer << argp_program_version << " | ";

    lBuffer << inCurrentFitness << " fitness";

    return lBuffer.str();
}

double embryoEvaluate(Embryo& iEmbryo, const double* inVector, size_t& outNbSteps, double& outSimilarity, bool ibColor) {

    iEmbryo.setupController(inVector);

    outNbSteps = 0;
    outSimilarity = 0.0;
    double lSimilaritySum = 0.0;
    double lSimilarity;
    size_t lNbStepsMax = iEmbryo.nbStepsMax();
    size_t lPicsNo = iEmbryo.getPicsNumber();
    size_t lNbSteps = 0;

    double lPenalty;

    double bestFitness = 1.0;
    double worstFitness = 0.0;

    size_t i = 0;
    while (i < lPicsNo) {
        if (!ibColor)
            iEmbryo.initCells(i);
        else
            iEmbryo.init(i);

        lNbSteps = 0;
        lSimilarity = 0.0;

        //#pragma omp parallel for private(counter, picColor)
        for (iEmbryo.resetMonitor(i); iEmbryo.update(ibColor) && (lNbSteps < iEmbryo.cellsNumber()); lNbSteps += 1);
        //        ibColor = !ibColor;
        //        if (lNbSteps < lNbStepsMax) {
        //            lSimilarity = iEmbryo.getSimilarity(i, ibColor);
        //            lNbStepsStructure = 0;
        //            for (iEmbryo.resetMonitor(i); iEmbryo.update(true) && (lNbStepsStructure < lNbStepsMax); lNbStepsStructure += 1);
        //        }

        //        if (lNbSteps < lNbStepsMax) {
        //            lSimilarity2 = iEmbryo.getSimilarity(i, ibColor);
        //            double lPenalty = double(lNbSteps + lNbStepsStructure) / (double) (2.0 * lNbStepsMax);
        //            double lPenalty2 = double(lNbSteps) / (double) (lNbStepsMax);
        //            lSimilarity *= (lPenalty * lPenalty + 1.0);
        //            lSimilarity2 *= (lPenalty2 * lPenalty2 + 1.0);
        ////            lSimilarity += lPenalty;
        //            //cout << "lSimilarity + penalty for " << i << " : " << lSimilarity << endl;
        //            lSimilarity += lSimilarity2;
        //            lSimilarity /= 2.0;
        //
        //        } else
        //            lSimilarity = 1.0;

        if (lNbSteps < lNbStepsMax) {
            lSimilarity = iEmbryo.getSimilarity(i, ibColor);
            double lPenalty = (double) ((double) (lNbSteps) / (double) (iEmbryo.cellsNumber()));
            lSimilarity *= (lPenalty * lPenalty + 1.0);
        } else
            lSimilarity = 1.0;

        if (lSimilarity < bestFitness)
            bestFitness = lSimilarity;
        if (lSimilarity > worstFitness)
            worstFitness = lSimilarity;

        lSimilaritySum += lSimilarity;
        outNbSteps += lNbSteps;
        lNbSteps = 0;

        ++i;
    }

    //    lPenalty = (double) (outNbSteps) / (double) (lPicsNo * lNbStepsMax);
    outSimilarity = (double) (lSimilaritySum / (double) lPicsNo);


    if (outSimilarity > 1) {
        outSimilarity = 1.0;
    } else if (outSimilarity < 0) {
        outSimilarity = 0.0;
    }

    return outSimilarity;

}

int
main(int argc, char* argv[]) {
    int tid;
    CmdLineConf lCmdLineConf;
    argp_parse(&argp, argc, argv, 0, 0, &lCmdLineConf);

    // Checks if some files are missing
    if (lCmdLineConf.testContentPath().empty()) {
        cerr << "No path specified" << endl;
        return EXIT_FAILURE;
    }
    if (lCmdLineConf.configFileName().empty()) {
        cerr << "No cmaes config file specified" << endl;
        return EXIT_FAILURE;
    }
    // Load the properties
    HandleT<Embryo> lEmbryo;
    Picture lInitPic;
    try {
        ifstream lFile;
        safeOpen(lFile, (lCmdLineConf.testContentPath() + "embryo.conf").c_str()); //lEmbryoConf);
        lEmbryo = Embryo::load(lFile, lCmdLineConf.seed(), (lCmdLineConf.testContentPath()).c_str(), bIsGuiOutputWindow);
        size_t lTargetIndex = 0;
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while loading embryo configuration file '"
                //<< lEmbryoConf
                << (lCmdLineConf.testContentPath() + "embryo.conf").c_str()
                << "' :\n  "
                << inException.getMessage()
                << endl;
        return EXIT_FAILURE;
    }

    pid_t myPid = getpid();
    try {
        ofstream lOutFile((lCmdLineConf.testContentPath() + "pid").c_str());
        lOutFile << myPid << endl;
        lOutFile.close(); //close the output file
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while saving process pid value to external file'"
                << "' :\n  "
                << inException.getMessage()
                << endl;
        return EXIT_FAILURE;
    }



    /*
     * Run CMAES (seal your belt and close your eyes)
     */

    // Init the CMAES state
    cmaes_t lCmaesState;
    double* lFitnessValues;
    try {
        lFitnessValues =
                cmaes_init(&lCmaesState,
                lEmbryo->controller().nbParameters(),
                NULL,
                NULL,
                lCmdLineConf.seed(),
                0,
                lCmdLineConf.configFileName().c_str());
        //        std::ostringstream outputString;
        //        outputString << lFitnessValues[0];
        //        cout << " \n lFitnessValues[0] contains: " << outputString.str();
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while cmaes init fitness value '"
                << "' :\n  "
                << inException.getMessage()
                << endl;
        return EXIT_FAILURE;
    }

    // Dump useful infos
    cout << "\n # " << cmaes_SayHello(&lCmaesState) << endl;

    // CMAES iterations
    size_t lPopSize = (size_t) cmaes_Get(&lCmaesState, "popsize");

    vector<size_t> lStructureSteps(lPopSize);
    vector<double> lSimilarities(lPopSize);
    cout << "# Population size: " << (int) lPopSize << endl;
    bool lFirstGeneration = true;
    double lBestEverSimilarity = 1.0;


    cout
            << "# generation\t"
            << "best fitness "
            << endl;

    char const* lStop;
    int lGen = 0;
    while ((lGen < 5) || (!(lStop = cmaes_TestForTermination(&lCmaesState)))) {
        // Generate a new point cloud
        //cout << "\n cmaes - test for termination \n";
        double* const* lPop = cmaes_SamplePopulation(&lCmaesState);

        // Evaluate the points
        for (size_t i = 0; i < lPopSize; i++) {
            size_t lNbSteps;
            double lSimilarity;

            Randomizer ran;
            size_t size = 1;
            ran.init(size);

            uint32_t ranInt = ran.uint32();
            size_t pic;
            pic = (size_t) (ranInt % 2);

            lFitnessValues[i] = embryoEvaluate(*lEmbryo, lPop[i], lNbSteps, lSimilarity, false);

            lSimilarities[i] = lFitnessValues[i]; // == lSimilarity;
            lStructureSteps[i] = lNbSteps;
            //            lSimilarities[i] = lSimilarity;
            //                    cout << "_pop: " << i << " | fit: " << lFitnessValues[i] << endl;
        }

        // Save the best genotype if it's the best ever
        size_t lBestIndex = 0;
        for (size_t i = 1; i < lPopSize; i++)
            if (lSimilarities[lBestIndex] > lSimilarities[i])
                lBestIndex = i;

        if (lFirstGeneration || (lBestEverSimilarity >= lSimilarities[lBestIndex])) {
            lBestEverSimilarity = lSimilarities[lBestIndex];

            cout << " best gen: " << lGen << " | best fit: " << lBestEverSimilarity << " | best index: " << lBestIndex << endl;
            ofstream lFile("best.cells.params");
            if (!lFile)
                cerr << "Unable to save the best genotype";

            lFile << lStructureSteps[lBestIndex];
            lFile << ' ';
            arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());

            //save x,y,size of cells also
            ofstream lFile2("cells.params");
            if (!lFile2)
                cerr << "Unable to save the cells properties";
            lEmbryo->saveCells(lFile2);

            lFile.close();
            lFile2.close();
        }

        lGen = cmaes_Get(&lCmaesState, "generation");
        //        if ((lGen % 20) == 0) {
        std::string lString;
        std::stringstream lStringStream;
        lStringStream << lGen;
        lString = lStringStream.str();
        lString = "best.cells.params." + lString;
        const char* lConstString = lString.c_str();
        ofstream lFile(lConstString);
        if (!lFile)
            cerr << "Unable to save the best genotype";
        lFile << lStructureSteps[lBestIndex];
        lFile << ' ';
        arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());

        lString = lStringStream.str();
        lString = "cells.params." + lString;
        lConstString = lString.c_str();
        ofstream lFile2(lConstString);
        if (!lFile2)
            cerr << "Unable to save the cells properties";
        lEmbryo->saveCells(lFile2);

        lFile.close();
        lFile2.close();
        //        }

        lFirstGeneration = false;

        // Update the CMAES state
        cmaes_UpdateDistribution(&lCmaesState, lFitnessValues);

        // Print stuffs
        //        cout
        //                << "#eval: "
        //                << cmaes_Get(&lCmaesState, "eval") << " | #gen: " // nb evals
        //                << lGen << " | best fit: " // nb generation
        //                << arrayd::minValue(lFitnessValues, lPopSize) // best generation fitness
        //                << endl << endl;
        cout
                << lGen << "\t" // nb generation
                << arrayd::minValue(lFitnessValues, lPopSize) // best generation fitness
                << endl;
    }

    // CMAES log
    cmaes_WriteToFile(&lCmaesState, "all", "allcmaes.dat");
    cmaes_exit(&lCmaesState);

    ifstream lFile;
    safeOpen(lFile, (lCmdLineConf.testContentPath() + "best.cells.params").c_str());
    lFile >> lEmbryo->structureSteps();
    arrayd::load(lFile, lEmbryo->controller().parameters(), lEmbryo->controller().nbParameters());
    lFile.close();

    //TODO: color for more cells
    lEmbryo->initCells(0);
    int lIterCounter = 0;
    size_t lNbStructureSteps = lEmbryo->structureSteps() + 10;
    while (lIterCounter < lNbStructureSteps) {
        lEmbryo->update(false);
        ++lIterCounter;
    }
    cout << "====" << lEmbryo->getSimilarity(0, false) << endl;

    ///////////////////////
    cout
            << "%% color optimization "
            << endl;

    cmaes_t lCmaesState2;
    double* lFitnessValues2;
    try {
        lFitnessValues2 =
                cmaes_init(&lCmaesState2,
                lEmbryo->controller().nbParameters(),
                NULL,
                NULL,
                lCmdLineConf.seed(),
                0,
                lCmdLineConf.configFileName().c_str());
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while cmaes init fitness value '"
                << "' :\n  "
                << inException.getMessage()
                << endl;
        return EXIT_FAILURE;
    }

    // CMAES iterations
    lPopSize = (size_t) cmaes_Get(&lCmaesState2, "popsize");

    vector<double> lSimilarities2(lPopSize);
    //cout << "# Population size: " << (int) lPopSize << endl;
    lFirstGeneration = true;
    lBestEverSimilarity = 1.0;

    while ((!(lStop = cmaes_TestForTermination(&lCmaesState2)))) {
        // Generate a new point cloud
        //cout << "\n cmaes - test for termination \n";
        double* const* lPop = cmaes_SamplePopulation(&lCmaesState2);

        for (size_t i = 0; i < lPopSize; i++) {
            size_t lNbSteps;
            double lSimilarity;

            Randomizer ran;
            size_t size = 1;
            ran.init(size);

            uint32_t ranInt = ran.uint32();
            size_t pic;
            pic = (size_t) (ranInt % 2);

            lFitnessValues2[i] = embryoEvaluate(*lEmbryo, lPop[i], lNbSteps, lSimilarity, true);

            lSimilarities2[i] = lSimilarity;
        }

        // Save the best genotype if it's the best ever
        size_t lBestIndex = 0;
        for (size_t i = 1; i < lPopSize; i++)
            if (lSimilarities2[lBestIndex] > lSimilarities2[i])
                lBestIndex = i;

        if (lFirstGeneration || (lBestEverSimilarity >= lSimilarities2[lBestIndex])) {
            lBestEverSimilarity = lSimilarities2[lBestIndex];

            ofstream lFile("best.params");
            if (!lFile)
                cerr << "Unable to save the best genotype";

            arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());

            lFile.close();
        }

        int lGen = cmaes_Get(&lCmaesState2, "generation");
        //        if (((int) lPopSize % 20) == 0) {
        //        if ((lGen % 20) == 0) {
        std::string lString;
        std::stringstream lStringStream;
        lStringStream << lGen;
        lString = lStringStream.str();
        lString = "best.params." + lString;
        const char* lConstString = lString.c_str();
        ofstream lFile(lConstString);
        if (!lFile)
            cerr << "Unable to save the best genotype";
        arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());


        lFile.close();
        //        }

        lFirstGeneration = false;

        // Update the CMAES state
        cmaes_UpdateDistribution(&lCmaesState2, lFitnessValues2);

        // Print stuffs
        //        cout
        //                << "#eval: "
        //                << cmaes_Get(&lCmaesState, "eval") << " | #gen: " // nb evals
        //                << lGen << " | best fit: " // nb generation
        //                << arrayd::minValue(lFitnessValues, lPopSize) // best generation fitness
        //                << endl << endl;
        cout
                << lGen << "\t" // nb generation
                << arrayd::minValue(lFitnessValues2, lPopSize) // best generation fitness
                << endl;
    }

    // CMAES log
    cmaes_WriteToFile(&lCmaesState2, "all", "allcmaes2.dat");
    cmaes_exit(&lCmaesState2);



    // Job done
    //    SDL_Quit();
    return EXIT_SUCCESS;
}
