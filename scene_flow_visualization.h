
#include <mrpt/utils.h>
#include <mrpt/system.h>
#include <mrpt/gui/CDisplayWindow3D.h>
#include <mrpt/opengl.h>
#include <OpenNI.h> //<openni2/OpenNI.h>
#include <Eigen/src/Core/Matrix.h>
#include "pdflow_cudalib.h"
#include "legend_pdflow.xpm"

#define M_LOG2E 1.44269504088896340736 //log2(e)

inline float log2(const float x){
    return  log(x) * M_LOG2E;
}


using namespace mrpt;
using namespace mrpt::math;
using namespace mrpt::utils;
using namespace std;
using mrpt::poses::CPose3D;
using Eigen::MatrixXf;


class PD_flow_mrpt {
public:

    float len_disp;         //In meters
    float fps;              //In Hz
    unsigned int cam_mode;	// (1 - 640 x 480, 2 - 320 x 240, 4 - 160 x 120)
    unsigned int ctf_levels;//Number of levels used in the coarse-to-fine scheme (always dividing by two)
    unsigned int num_max_iter[6];  //Max number of iterations distributed homogeneously between all levels
    MatrixXf g_mask;
	
    //Matrices that store the original images with the image resolution
    MatrixXf colour_wf;
    MatrixXf depth_wf;

    //Matrices that store the images downsampled
    vector<MatrixXf> colour;
    vector<MatrixXf> colour_old;
    vector<MatrixXf> depth;
    vector<MatrixXf> depth_old;
    vector<MatrixXf> xx;
    vector<MatrixXf> xx_old;
    vector<MatrixXf> yy;
    vector<MatrixXf> yy_old;

    //Motion field
    vector<MatrixXf> dx;
    vector<MatrixXf> dy;
    vector<MatrixXf> dz;

    //Camera properties
    float f_dist;	//In meters
    float fovh;     //Here it is expressed in radians
    float fovv;     //Here it is expressed in radians

    //Max resolution of the coarse-to-fine scheme.
    unsigned int rows;
    unsigned int cols;

    //Optimization Parameters
    float mu, lambda_i, lambda_d;

    //Visual
    gui::CDisplayWindow3D       window;
    opengl::COpenGLScenePtr		scene;
    utils::CImage				image;

    //Camera
    openni::Status          rc;
    openni::Device          device;
    openni::VideoMode       options;
    openni::VideoStream 	rgb,dimage;

    //Cuda
    CSF_cuda csf_host, *csf_device;


	//Methods
    void createImagePyramidGPU();
    void solveSceneFlowGPU();
	bool OpenCamera();
	void CloseCamera();
	void CaptureFrame();
    void freeGPUMemory();
    void initializeCUDA();
	void initializeScene();
	void updateScene();
	void initializePDFlow();

    PD_flow_mrpt(unsigned int cam_mode_config, unsigned int fps_config, unsigned int rows_config);
};


