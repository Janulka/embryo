#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <argp.h>
#include <algorithm>
#include "SDL.h"

#include "ArrayOps.h"
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
//int gVideoFlags = SDL_HWSURFACE | SDL_ANYFORMAT | SDL_HWPALETTE| SDL_DOUBLEBUF | SDL_RESIZABLE;

class CmdLineConf {
    size_t mWidth, mHeight;
    std::string mEmbryoFileName;
    std::string mParamFileName;
    std::string mCellFileName;
    std::string mTargetFileName;
    std::string mTestPath;
public:

    CmdLineConf() : mWidth(0), mHeight(0) {
    }

    inline const size_t width() const {
        return mWidth;
    }

    inline size_t& width() {
        return mWidth;
    }

    inline const size_t height() const {
        return mWidth;
    }

    inline size_t& height() {
        return mHeight;
    }

    inline const std::string& embryoFileName() const {
        return mEmbryoFileName;
    }

    inline std::string& embryoFileName() {
        return mEmbryoFileName;
    }

    inline const std::string& paramFileName() const {
        return mParamFileName;
    }

    inline std::string& paramFileName() {
        return mParamFileName;
    }

    inline const std::string& cellFileName() const {
        return mCellFileName;
    }

    inline std::string& cellFileName() {
        return mCellFileName;
    }

    inline const std::string& targetFileName() const {
        return mTargetFileName;
    }

    inline std::string& targetFileName() {
        return mTargetFileName;
    }

    inline const std::string& testContentPath() const {
        return mTestPath;
    }

    inline std::string& testContentPath() {
        return mTestPath;
    }
}
; // struct CmdLineConf



const char* argp_program_version = "embryo-view";

const char *argp_program_bug_address = "<janahlava@gmail.com>";

static char doc[] = "embryo-view -- Viewer for embryogenic flags";

static struct argp_option options[]
        = {
    {
        "embryo", 'e', "EMBRYO", 0,
        "Embryo configuration file"
    },
    {
        "parameters", 'p', "PARAMETERS", 0,
        "Parameters file"
    },
    {
        "cells", 's', "CELLS", 0,
        "Cells parameters file"
    },
    {
        "width", 'w', "WIDTH", 0,
        "Width of the embryo (override the one from the embryo configuration files)"
    },
    {
        "height", 'h', "HEIGHT", 0,
        "Height of the embryo (override the one from the embryo configuration files)"
    },
    {
        "target", 't', "TARGET", 0,
        "Target picture (as a PGM file)"
    },
    {
        "path", 'c', "PATH", 0,
        "test folder path"
    },
    {
        0
    }
};

template <typename T>
static T
str2(const char* inStr) {
    T lValue;
    istringstream lBuffer(inStr);
    lBuffer >> lValue;
    return lValue;
}

static error_t
parseCmdLineOptions(int key, char* arg, struct argp_state* state) {
    CmdLineConf* lCmdLineConf = (CmdLineConf*) state->input;

    switch (key) {
        case 'e':
            lCmdLineConf->embryoFileName() = arg;
            break;

        case 'p':
            lCmdLineConf->paramFileName() = arg;
            break;

        case 's':
            lCmdLineConf->cellFileName() = arg;
            break;

        case 'w':
            lCmdLineConf->width() = str2<size_t > (arg);
            break;

        case 'h':
            lCmdLineConf->height() = str2<size_t > (arg);
            break;

        case 't':
            lCmdLineConf->targetFileName() = arg;
            break;

        case 'c':
            lCmdLineConf->testContentPath() = arg;
            if (lCmdLineConf->testContentPath()[lCmdLineConf->testContentPath().size() - 1] != '/')
                lCmdLineConf->testContentPath() += '/';
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

void
safeOpen(ifstream& inFile, const std::string& inFileName) {
    inFile.open(inFileName.c_str());
    if (!inFile.good()) {
        cerr
                << "Unable to open '"
                << inFileName
                << "'"
                << endl;
        exit(EXIT_FAILURE);
    }
}

void
savePicture(Picture& inPic, const std::string& inPicFileName) {
    try {
        if (!(inPicFileName.empty())) {
            ofstream lFile((inPicFileName + ".pgm").c_str());
            savePGM(lFile, inPic);
        }
    } catch (Exception& inException) {
        cerr
                << "Error while writing 'embryo.pgm' :\n"
                << inException.getMessage()
                << endl;
        exit(EXIT_FAILURE);
    }
}

std::string
buildWindowTitle(size_t inNbEpochs, double inCurrentFitness, bool inRunMode) {
    ostringstream lBuffer;

    lBuffer << argp_program_version << " | ";

    lBuffer << inNbEpochs << " epochs";

    lBuffer << argp_program_version << " | ";

    lBuffer << inCurrentFitness << " fitness";

    if (!inRunMode)
        lBuffer << " (pause)";

    return lBuffer.str();
}

int main(int argc, char* argv[]) {
    // Parse the command line
    CmdLineConf lCmdLineConf;
    argp_parse(&argp, argc, argv, 0, 0, &lCmdLineConf);

    Picture lTargetPic;
    double lMaxDist = -1.0;

    std::string lTargetFileName = (lCmdLineConf.testContentPath() + lCmdLineConf.targetFileName()).c_str();
    if (!(lCmdLineConf.targetFileName().empty())) {
        // Load the target picture
        try {
            ifstream lFile;
            safeOpen(lFile, lTargetFileName);
            loadPGM(lFile, lTargetPic);
            lMaxDist = maxDistance(lTargetPic);
        } catch (embryo::Exception& inException) {
            cerr
                    << "Error while loading picture to compare'"
                    << lCmdLineConf.targetFileName()
                    << "' :\n  "
                    << inException.getMessage()
                    << endl;
            //return EXIT_FAILURE;
        }
    }


    // Checks if some files are missing
    if (lCmdLineConf.embryoFileName().empty()) {
        cerr << "No embryo file specified" << endl;
        return EXIT_FAILURE;
    }
    std::string lEmbryoFileName = lCmdLineConf.embryoFileName();

    // Load the embryo
    HandleT<Embryo> lEmbryo;
    try {
        ifstream lFile;
        safeOpen(lFile, (lCmdLineConf.testContentPath() + lCmdLineConf.embryoFileName()).c_str());
        bool obGui;
        lEmbryo = Embryo::load(lFile, 0, (lCmdLineConf.testContentPath()).c_str(), obGui);
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while loading embryo conf '"
                << (lCmdLineConf.testContentPath() + lCmdLineConf.embryoFileName()).c_str()
                << "' :\n  "
                << inException.getMessage()
                << endl;
        return EXIT_FAILURE;
    }

    //TODO: reshape
    //  // Reshape the embryo if asked for it
    //  size_t lDesiredWidth = lEmbryo->width();
    //  size_t lDesiredHeight = lEmbryo->height();
    //
    //  if (lCmdLineConf.width() != 0)
    //    lDesiredWidth = lCmdLineConf.width();
    //
    //  if (lCmdLineConf.height() != 0)
    //    lDesiredHeight = lCmdLineConf.height();
    //
    //  lEmbryo->reshape(lDesiredWidth, lDesiredHeight);

    //load cells
    try {
        ifstream lFile;
        safeOpen(lFile, (lCmdLineConf.testContentPath() + lCmdLineConf.cellFileName()).c_str());
        lFile >> lEmbryo->structureSteps();
        arrayd::load(lFile, lEmbryo->controller().parameters(), lEmbryo->controller().nbParameters());
        //        lEmbryo->loadCells(lFile);
    } catch (embryo::Exception& inException) {
        cerr
                << "Error while loading cells params file '"
                << (lCmdLineConf.testContentPath() + lCmdLineConf.cellFileName()).c_str()
                << "' :\n"
                << inException.getMessage()
                << endl;
        return EXIT_FAILURE;
    }



    // Setup a randomizer
    Randomizer lRandomizer;
    lRandomizer.init(1985);


    vector<EmbryoView*> evChem;
    vector<EmbryoView*> evStat;
    vector<Picture*> picChem;
    vector<Picture*> picStat;

    // Setup the user interface
    WidgetContainer lWidgetContainer;

    lWidgetContainer.push();

    lWidgetContainer.push();
    size_t lIndex = lEmbryo->getTargetIndex(lTargetFileName);
    EmbryoView * evColor = new EmbryoView(*lEmbryo, new ColourAspect(), lIndex);
    Picture * lPicColorP = evColor->getPictureP();
    lWidgetContainer.addWidget(evColor);
    lWidgetContainer.pop();

    lWidgetContainer.pop();

    // SDL initialization
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        cerr << "Unable to init SDL: " << SDL_GetError();
        exit(EXIT_FAILURE);
    }

    SDL_Surface* lScreen =
            SDL_SetVideoMode(gWidth, gHeight, 32, gVideoFlags);

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


    // Main event loop
    bool lRunMode = true;
    bool lScrambleMode = false;
    bool lPaintRequested = true;
    size_t lNbUpdates = 0;

    double outSimilarity = -1.0;
    SDL_WM_SetCaption(buildWindowTitle(lNbUpdates, outSimilarity, lRunMode).c_str(), "ICON");

    //TODO:


    size_t pathLength;
    pathLength = lTargetFileName.find_last_of("/\\");
    cout << " folder: " << lTargetFileName.substr(0, pathLength) << endl;
    string lPicName = lTargetFileName.substr(pathLength + 1);
    cout << " file: " << lPicName << endl;

    size_t nameLength = lPicName.find_last_of(".");
    string lRawName = lPicName.substr(0, nameLength);
    cout << " name: " << lRawName << endl;
    string lSuffix = lPicName.substr(nameLength + 1);
    cout << " suffix: " << lSuffix << endl;

    size_t lBorder = lEmbryo->getBorderValue(lRawName);
   
    cout << lBorder << " lEmbryo->init " << lPicName << endl;
    lEmbryo->initCells(lBorder);

    const int lHistorySize = 10;
    vector<double> lSimilarities(lHistorySize);
    for (int i = 0; i < lHistorySize; i++)
        lSimilarities[i] = 4;

    size_t lIterCounter = 0;
    bool bColor = false;
    size_t lStructureSteps = lEmbryo->structureSteps() + 10;

    for (bool lMustQuit = false; !lMustQuit;) {
        if ((!bColor) && (lIterCounter >= lStructureSteps)) {
            cout << " COLOR evaluation" << endl;
            lEmbryo->init(lBorder);
            bColor = true;
            // Load the model parameters
            try {
                ifstream lFile;
                safeOpen(lFile, (lCmdLineConf.testContentPath() + lCmdLineConf.paramFileName()).c_str());
                arrayd::load(lFile, lEmbryo->controller().parameters(), lEmbryo->controller().nbParameters());
            } catch (embryo::Exception& inException) {
                cerr
                        << "Error while loading NN controller params '"
                        << (lCmdLineConf.testContentPath() + lCmdLineConf.paramFileName()).c_str()
                        << "' :\n"
                        << inException.getMessage()
                        << endl;
                return EXIT_FAILURE;
            }
        }
        SDL_Event lEvent;
        while (SDL_PollEvent(&lEvent)) {
            switch (lEvent.type) {
                case SDL_KEYDOWN:
                    switch (lEvent.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            lMustQuit = true;
                            break;

                        case SDLK_RIGHT:
                            if (!lRunMode) {
                                lEmbryo->update(bColor);
                                lNbUpdates += 1;
                                lPaintRequested = true;
                                SDL_WM_SetCaption(buildWindowTitle(lNbUpdates, outSimilarity, lRunMode).c_str(), "ICON");
                                int counter = 0;
                                lEmbryo->getColours(*lPicColorP);
                                vector<Picture*>::iterator itr = picChem.begin();
                                while (itr != picChem.end()) {
                                    lEmbryo->getChemicals(**itr, counter++);
                                    ++itr;
                                }
                                counter = 0;
                                itr = picStat.begin();
                                while (itr != picStat.end()) {
                                    lEmbryo->getStates(**itr, counter++);
                                    ++itr;
                                }

                                lWidgetContainer.paint(lScreen, bColor);

                                int update = SDL_Flip(lScreen);
                                if (update == -1) {
                                    cerr << "error by painting " << SDL_GetError() << std::endl;
                                    return EXIT_FAILURE;
                                }

                                lIterCounter++;
                            }
                            break;

                        case SDLK_p: //print active window client
                            lEmbryo->getColours(*lPicColorP);
                            savePicture(*lPicColorP, "embryo");
                            break;

                        case SDLK_SPACE:
                            lRunMode = !lRunMode;
                            SDL_WM_SetCaption(buildWindowTitle(lNbUpdates, outSimilarity, lRunMode).c_str(), "ICON");
                            break;

                        case SDLK_n:
                            lScrambleMode = true;
                            break;

                        case SDLK_s: //s == structure switch
                            bColor = !bColor;
                            SDL_WM_SetCaption("structure", "ICON");
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_KEYUP:
                    switch (lEvent.key.keysym.sym) {
                        case SDLK_n:
                            lScrambleMode = false;
                            break;

                        default:
                            break;
                    }
                    break;

                case SDL_VIDEORESIZE:
                    lScreen =
                            SDL_SetVideoMode(lEvent.resize.w, lEvent.resize.h, 32, gVideoFlags);
                    lPaintRequested = true;
                    break;

                case SDL_QUIT:
                    lMustQuit = true;
                    break;

                default:
                    break;
            }
        }

        if (lScrambleMode) {
            lEmbryo->scramble(lRandomizer, 1.0);
            lPaintRequested = true;
        }

        if (lRunMode) {
            lEmbryo->update(bColor);
            lNbUpdates += 1;
            lPaintRequested = true;

            SDL_WM_SetCaption(buildWindowTitle(lNbUpdates, outSimilarity, lRunMode).c_str(), "ICON");

            if (lMaxDist > 0) { 
                lEmbryo->getColours(*lPicColorP);

                //TODO: compute the fitness
                if (bColor)                    
                    outSimilarity = embryo::distance(*lPicColorP, lTargetPic) / lMaxDist;
                else
                    outSimilarity = embryo::distanceStructure(*lPicColorP, lTargetPic);
            }


            SDL_WM_SetCaption(buildWindowTitle(lNbUpdates, outSimilarity, lRunMode).c_str(), "ICON");
            if (lPaintRequested) {
                int counter = 0;
                lEmbryo->getColours(*lPicColorP);
                vector<Picture*>::iterator itr = picChem.begin();
                while (itr != picChem.end()) {
                    lEmbryo->getChemicals(**itr, counter++);
                    ++itr;
                }
                counter = 0;
                itr = picStat.begin();
                while (itr != picStat.end()) {
                    lEmbryo->getStates(**itr, counter++);
                    ++itr;
                }

                lWidgetContainer.paint(lScreen, bColor);

                int update = SDL_Flip(lScreen);
                if (update == -1) {
                    cerr << "error by painting " << SDL_GetError() << std::endl;
                    return EXIT_FAILURE;
                }

                lPaintRequested = false;
            }
            lIterCounter++;
        }

        SDL_Delay(10);
    }

    // Job done
    SDL_Quit();
    return EXIT_SUCCESS;
}

