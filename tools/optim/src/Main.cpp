#include <stdint.h>
#include <argp.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <embryo.h>
#include <sys/types.h>
#include <unistd.h>
#include <iomanip>
#include <set>
#include <signal.h>

#include "SDL/SDL.h"
#include <ArrayOps.h>
#include <omp.h>

#include "cmaes/cmaes_interface.h"

#include "Widget.h"
#include "WidgetContainer.h"
#include "EmbryoView.h"
#include "ChemicalAspect.h"
#include "ColourAspect.h"
#include "StateAspect.h"
#include "Picture.h"

using namespace std;
using namespace embryo;

int gWidth = 512;
int gHeight = 512;
int gVideoFlags = SDL_SWSURFACE | SDL_RESIZABLE;
bool bIsGui; // = false;
bool bIsGuiOutputWindow;
bool bSaveParamsEachGeneration;

class exceptionPics : public std::exception {
public:

    virtual const char* what() const throw () {
        return "more than one pic on input";
    }
};

class exceptionCounting : public std::exception {
public:

    virtual const char* what() const throw () {
        return "exception in sorting fintesses";
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

const char *argp_program_bug_address = "<baran.jana@gmail.com>";

static char doc[] = "embryo-optim -- CMAES optimizer for cellular embryogenetic flags";

static struct argp_option options[]
        = {
    {
        "seed", 's', "SEED", 0,
        "Random seed value"
    },
    {
        "path", 'p', "PATH", 0,
        "test folder path with embryo configuration file and pictures files"
    },
    {
        "config", 'c', "CONFIG", 0,
        "CMAES configuration file"
    },
    {
        "NN weight per each generation", 'g', 0, 0,
        "use the argument if you like to save MLP params for each generation"
    },
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
            if (lCmdLineConf->testContentPath()[lCmdLineConf->testContentPath().size() - 1] != '/')
                lCmdLineConf->testContentPath() += '/';
            break;

	case 'g':
            bSaveParamsEachGeneration = true;
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

int safeOpen(ifstream& inFile, const std::string& inFileName) {
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

std::string buildWindowTitle(size_t inNbFitness, double inCurrentFitness) {
    ostringstream lBuffer;

    lBuffer << argp_program_version << " | ";

    lBuffer << inNbFitness << " pop ";

    lBuffer << argp_program_version << " | ";

    lBuffer << inCurrentFitness << " fitness";

    return lBuffer.str();
}

static bool aborted = false;

static void exit_sig_handler(int sig) {
    if (aborted) {
        std::cerr << "Forcing exit!!!" << std::endl;
        std::exit(EXIT_FAILURE);
    } else {
        std::cerr << "Caught signal " << strsignal(sig) << "(" << sig << "), finishing... (Ctrl+C again to force exit)" << std::endl;
        aborted = true;
    }
}

double embryoEvaluate(Embryo& iEmbryo, const double* inVector, size_t& outNbSteps, double& outSimilarity, Picture*& picColor, SDL_Surface* lScreen, WidgetContainer & iWidgetContainer, bool ibColor)
{
    iEmbryo.setupController(inVector);

    outNbSteps = 0;
    outSimilarity = 0.0;
    double lSimilaritySum = 0.0;
    double lSimilarity;
    size_t lNbStepsMax = iEmbryo.nbStepsMax();
    size_t lPicsNo = iEmbryo.getPicsNumber();
    size_t lNbSteps = 0;

    //double lPenalty = double(outNbSteps) / (lPicsNo * lNbStepsMax);

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

        for (iEmbryo.resetMonitor(i); iEmbryo.update(ibColor) && (lNbSteps < lNbStepsMax); lNbSteps += 1);

        SDL_PumpEvents();
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if (keystate[SDLK_g]) {
            bIsGui = !bIsGui;
            keystate[SDLK_g] = '\0';
        } else if (keystate[SDLK_p]) {
            int input;
            cin >> input;
            keystate[SDLK_p] = '\0';
            cout << "___" << iEmbryo.getSimilarity(i, ibColor) << endl;
        } else if (keystate[SDLK_ESCAPE])
            exit(1);

        if (bIsGui) {
            iEmbryo.getColours(*picColor);
            iWidgetContainer.paint(lScreen, ibColor);

            int update = SDL_Flip(lScreen);
            if (update == -1) {
                cerr << "error by painting " << SDL_GetError() << std::endl;
                exit(0);
            }

        }

        if (lNbSteps < lNbStepsMax && !aborted) {
            lSimilarity = iEmbryo.getSimilarity(i, ibColor);
            double lPenalty = (double) ((double) (lNbSteps) / (double) (lNbStepsMax));
            lSimilarity *= (lPenalty * lPenalty + 1.0);
        } else {
            lSimilarity = 1.0;
        }

        if (lSimilarity < bestFitness)
            bestFitness = lSimilarity;
        if (lSimilarity > worstFitness)
            worstFitness = lSimilarity;

        lSimilaritySum += lSimilarity;
        outNbSteps += lNbSteps;
        lNbSteps = 0;

        ++i;
    }

    outSimilarity = lSimilaritySum / (double) lPicsNo;

    if (outSimilarity > 1) {
        outSimilarity = 1.0;
    } else if (outSimilarity < 0) {
        outSimilarity = 0.0;
    }

    return outSimilarity;

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

    //double lPenalty;

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

        for (iEmbryo.resetMonitor(i); iEmbryo.update(ibColor) && (lNbSteps < lNbStepsMax); lNbSteps += 1);

        if (lNbSteps < lNbStepsMax && !aborted) {
            lSimilarity = iEmbryo.getSimilarity(i, ibColor);
            double lPenalty = (double) ((double) (lNbSteps) / (double) (lNbStepsMax));
            lSimilarity *= (lPenalty * lPenalty + 1.0);
        } else {
            lSimilarity = 1.0;
        }

        if (lSimilarity < bestFitness)
            bestFitness = lSimilarity;
        if (lSimilarity > worstFitness)
            worstFitness = lSimilarity;

        lSimilaritySum += lSimilarity;
        outNbSteps += lNbSteps;
        lNbSteps = 0;

        ++i;
    }

    outSimilarity = (double) (lSimilaritySum / (double) lPicsNo);


    if (outSimilarity > 1) {
        outSimilarity = 1.0;
    } else if (outSimilarity < 0) {
        outSimilarity = 0.0;
    }

    return outSimilarity;

}

int main(int argc, char* argv[]) {

    // install signal handler
    struct sigaction new_action, old_action;
    new_action.sa_handler = exit_sig_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = SA_RESTART;

    sigaction(SIGINT, NULL, &old_action);// ctrl+c
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGINT, &new_action, NULL);
    }
    sigaction(SIGTERM, NULL, &old_action);// kill (non -9)
    if (old_action.sa_handler != SIG_IGN) {
        sigaction(SIGTERM, &new_action, NULL);
    }

    bSaveParamsEachGeneration = false;
    bIsGui = false;
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
        safeOpen(lFile, (lCmdLineConf.testContentPath() + "embryo.conf").c_str());
        lEmbryo = Embryo::load(lFile, lCmdLineConf.seed(), (lCmdLineConf.testContentPath()).c_str(), bIsGuiOutputWindow);
    } catch (embryo::Exception& inException) {
        cerr
            << "Error while loading embryo configuration file '"
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


    // GUI output
    Picture * lPicColorP;
    WidgetContainer lWidgetContainer;
    SDL_Surface* lScreen = NULL;
    if (bIsGuiOutputWindow) {
        lWidgetContainer.push();

        lWidgetContainer.push();

        EmbryoView * evColor = new EmbryoView(*lEmbryo, new ColourAspect(), 0);
        lPicColorP = evColor->getPictureP();
        lWidgetContainer.addWidget(evColor);

        lWidgetContainer.pop();

        lWidgetContainer.pop();

        // SDL initialization
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
            cerr << "Unable to init SDL: " << SDL_GetError();
            exit(EXIT_FAILURE);
        }

        lScreen = SDL_SetVideoMode(gWidth, gHeight, 32, gVideoFlags);

        if (lScreen != NULL) {
            std::cout << "The current video surface bits per pixel is " << (int) lScreen->format->BitsPerPixel << std::endl;
        } else {
            std::cerr << "Video initialization failed: " << SDL_GetError() << std::endl;
            return EXIT_FAILURE;
        }

        if (!lScreen) {
            cerr
                << "Unable to set " << gWidth << 'x' << gHeight
                << " video : " << SDL_GetError()
                << endl;
            return EXIT_FAILURE;
        }

        //initial painting - target color pattern
        lWidgetContainer.paint(lScreen, true);

    }
    // GUI output

    size_t lPopSize;
    bool lFirstGeneration;
    double lBestEverSimilarity;
    char const* lStop;

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
    lPopSize = (size_t) cmaes_Get(&lCmaesState, "popsize");

    vector<size_t> lStructureSteps(lPopSize);
    vector<double> lSimilarities(lPopSize);
    cout << "# Population size: " << (int) lPopSize << endl;
    lFirstGeneration = true;
    lBestEverSimilarity = 1.0;


    cout
        << "# generation\t"
        << "best fitness "
        << endl;

    int lGen = 0;
    while ((lGen < 5) || (!(lStop = cmaes_TestForTermination(&lCmaesState)))) {
        if (aborted) {
            aborted = false;
            break;
        }
        // Generate a new point cloud
        double* const* lPop = cmaes_SamplePopulation(&lCmaesState);

        // Evaluate the points
        for (size_t i = 0; i < lPopSize; i++) {
            size_t lNbSteps;
            double lSimilarity;

            //Randomizer ran;
            //size_t size = 1;
            //ran.init(size);

            //uint32_t ranInt = ran.uint32();
            //size_t pic;
            //pic = (size_t) (ranInt % 2);

            if (bIsGuiOutputWindow && lScreen != NULL) {
                if (bIsGui)
                    SDL_WM_SetCaption(buildWindowTitle(i, lFitnessValues[i]).c_str(), "ICON");
                lFitnessValues[i] = embryoEvaluate(*lEmbryo, lPop[i], lNbSteps, lSimilarity, lPicColorP, lScreen, lWidgetContainer, false); 
            } else {
                lFitnessValues[i] = embryoEvaluate(*lEmbryo, lPop[i], lNbSteps, lSimilarity, false);
            }

            lSimilarities[i] = lFitnessValues[i]; // == lSimilarity;
            lStructureSteps[i] = lNbSteps;
            //                    cout << "_pop: " << i << " | fit: " << lFitnessValues[i] << endl;
        }

        // Save the best genotype if it's the best ever
        size_t lBestIndex = 0;
        for (size_t i = 1; i < lPopSize; i++)
            if (lSimilarities[lBestIndex] >= lSimilarities[i])
                lBestIndex = i;

        if (lFirstGeneration || (lBestEverSimilarity >= lSimilarities[lBestIndex])) {
            lBestEverSimilarity = lSimilarities[lBestIndex];

            cout << " best gen: " << lGen << " | best fit: " << lBestEverSimilarity << " | best index: " << lBestIndex << endl;
            ofstream lFile((lCmdLineConf.testContentPath() + "best.cells.params").c_str());
            if (!lFile)
                cerr << "Unable to save the best genotype";

            lFile << lStructureSteps[lBestIndex];
            lFile << ' ';
            arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());

            //save x,y,size of cells also
            ofstream lFile2((lCmdLineConf.testContentPath() + "cells.params").c_str());
            if (!lFile2)
                cerr << "Unable to save the cells properties";
            lEmbryo->saveCells(lFile2);

            lFile.close();
            lFile2.close();
        }

        lGen = cmaes_Get(&lCmaesState, "generation");
        //save best NN weights for current generation
        if (bSaveParamsEachGeneration) {
            std::stringstream lStringStream;
            lStringStream << lGen;
            std::string lString = lCmdLineConf.testContentPath() + "best.cells.params." + lStringStream.str();
            const char* lConstString = lString.c_str();
            ofstream lFile(lConstString);
            if (!lFile)
                cerr << "Unable to save the best genotype";
            lFile << lStructureSteps[lBestIndex];
            lFile << ' ';
            arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());

            lString = lCmdLineConf.testContentPath() + "cells.params." + lStringStream.str();
            lConstString = lString.c_str();
            ofstream lFile2(lConstString);
            if (!lFile2)
                cerr << "Unable to save the cells properties";
            lEmbryo->saveCells(lFile2);

            lFile.close();
            lFile2.close();
        }

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


        bool bStop = true;
        int inN = 1;
        if (bIsGui) {
            SDL_PumpEvents();
            while (bStop) {
                lEmbryo->getColours(*lPicColorP);
                lWidgetContainer.paint(lScreen, false);

                //  SDL_UpdateRect(lScreen, 0, 0, 0, 0);

                int update = SDL_Flip(lScreen);
                if (update == -1) {
                    cerr << "error by painting " << SDL_GetError() << std::endl;
                    return EXIT_FAILURE;
                }
                double lF = lEmbryo->getSimilarity(0, false);
                SDL_WM_SetCaption(buildWindowTitle(0, lF).c_str(), "ICON");
                //                cout << "====" << lEmbryo->getSimilarity(0, false) << endl;

                cin >> inN;
                if (inN == 0) {
                    inN = 1;
                    bStop = false;
                }
            }
        }
    }

    // CMAES log
    cmaes_WriteToFile(&lCmaesState, "all", "allcmaes.dat");
    cmaes_exit(&lCmaesState);

    ifstream lFile;
    if (safeOpen(lFile, (lCmdLineConf.testContentPath() + "best.cells.params").c_str()) == EXIT_SUCCESS) {
        lFile >> lEmbryo->structureSteps();
        arrayd::load(lFile, lEmbryo->controller().parameters(), lEmbryo->controller().nbParameters());
        lFile.close();
    }

    //TODO: color for more cells
    lEmbryo->initCells(0);
    size_t lIterCounter = 0;
    size_t lNbStructureSteps = lEmbryo->structureSteps() + 10;
    while (lIterCounter < lNbStructureSteps) {
        lEmbryo->update(false);
        ++lIterCounter;
    }
    cout << "====" << lEmbryo->getSimilarity(0, false) << endl;

    ///////////////////////


    //    lEmbryo->initCells(0);
    //
    //    int lIterCounter = 0;
    //    bool bColor = false;
    //    size_t lStructureStepss = lEmbryo->structureSteps() + 10;
    //    while (lIterCounter < lStructureStepss) {
    //        lEmbryo->update(bColor);
    //        lIterCounter++;
    //    }

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
        if (aborted) {
            break;
        }
        // Generate a new point cloud
        double* const* lPop = cmaes_SamplePopulation(&lCmaesState2);

        for (size_t i = 0; i < lPopSize; i++) {
            size_t lNbSteps;
            double lSimilarity;

            //Randomizer ran;
            //size_t size = 1;
            //ran.init(size);

            //uint32_t ranInt = ran.uint32();
            //size_t pic;
            //pic = (size_t) (ranInt % 2);

            if (bIsGuiOutputWindow) {
                if (bIsGui)
                    SDL_WM_SetCaption(buildWindowTitle(i, lFitnessValues2[i]).c_str(), "ICON");
                lFitnessValues2[i] = embryoEvaluate(*lEmbryo, lPop[i], lNbSteps, lSimilarity, lPicColorP, lScreen, lWidgetContainer, true); //, lEmbryo->mTargetPic);
            } else {
                lFitnessValues2[i] = embryoEvaluate(*lEmbryo, lPop[i], lNbSteps, lSimilarity, true);
            }

            lSimilarities2[i] = lSimilarity;
        }

        // Save the best genotype if it's the best ever
        size_t lBestIndex = 0;
        for (size_t i = 1; i < lPopSize; i++)
            if (lSimilarities2[lBestIndex] >= lSimilarities2[i])
                lBestIndex = i;

        if (lFirstGeneration || (lBestEverSimilarity >= lSimilarities2[lBestIndex])) {
            lBestEverSimilarity = lSimilarities2[lBestIndex];

            ofstream lFile((lCmdLineConf.testContentPath() + "best.params").c_str());
            if (!lFile)
                cerr << "Unable to save the best genotype";

            arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());

            lFile.close();
        }

        int lGen = cmaes_Get(&lCmaesState2, "generation");
        //save best NN weights for current generation
        if (bSaveParamsEachGeneration) {
            std::stringstream lStringStream;
            lStringStream << lGen;
            std::string lString = lCmdLineConf.testContentPath() + "best.params." + lStringStream.str();
            const char* lConstString = lString.c_str();
            ofstream lFile(lConstString);
            if (!lFile)
                cerr << "Unable to save the best genotype";
            arrayd::save(lFile, lPop[lBestIndex], lEmbryo->controller().nbParameters());


            lFile.close();
        }

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
