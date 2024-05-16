#ifndef CORE_ML_H_
#define CORE_ML_H_

#ifdef MLDLL_EXPORT
#define ML_EXPORT_API __declspec(dllexport)
#else 
#define ML_EXPORT_API __declspec(dllimport)
#endif


#ifndef IN
#define IN
#endif // !IN

#ifndef OUT
#define OUT
#endif // !OUT

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL   ((void *)0)
#endif
#endif

#ifndef PI
#define PI  3.14159265358979323846
#endif


#ifndef VERTICAL
#define VERTICAL 1
#endif


#ifndef HORIZONTAL
#define HORIZONTAL 0
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)	if(p){ delete (p); (p) = NULL;} 
#endif //SAFE_DELETE

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p)	if(p){ delete[] (p); (p) = NULL;}
#endif //SAFE_DELETE_ARRAY

#include<string>
#include<map>
#include <vector>

namespace CORE {

    enum Rotation
    {
        R0,        //Rotate 0
        R90,      //Rotate 90
        R180,  //Rotate 180
        R270  //Rotate 270
    };

    enum Color
    {
        R,
        G,
        B,
        W
    };

    enum Mirror
    {
        LEFT_RIGHT,
        UP_DOWN,
        NO_OP
    };

    enum ColorFilter
    {
        X,
        Y,
        Z,
        ColorBLOCK,
        ColorEMPTY
    };

    enum ND_Filter
    {
        ND0,
        ND1,
        ND2,
        ND3,
        ND_BLOCK,
        ND_EMPTY
    };

    enum WrapperStatus
    {
        Wrapper_Init = 0,

        Wrapper_Start = 1,

        Wrapper_Done,

        Wrapper_Failed,

        Wrapper_Motion_Error,

        Wrapper_Camera_Error,

        Wrapper_Param_Error,

        Wrapper_AutoAOI_Error,

        Wrapper_Image_Invalid,

        Wrapper_Session_Get_DUT_Info,

        Wrapper_Session_start,

        Wrapper_Session_stop,

        Wrapper_Session_save,

        Wrapper_Error,

        Wrapper_Error_fatal,

        Wrapper_Metrics_Cal_Error,

        Wrapper_Test_Result
    };

    typedef enum ML_AlignmentStatus
    {
        STATUS_CAMERA_UNOPEN = 1,

        STATUS_CAMERA_FAILED = 2,

        STATUS_IMAGE_NULL = 3,

        STATUS_POINT_ERROR = 4,

        STATUS_OK = 5,

        STATUS_KEYENCE_UNCONNECT = 6,

        STATUS_MOTION_UNCONNECT = 7,

        SAVE_FAILED = 8,

        STATUS_FINISHED

    }ML_AlignmentStatus;

    enum DUT_TYPE
    {
        DUT_DPAL = 0,

        DUT_DPAR = 1,

        DUT_bDPA = 2,

        DUT_bDPA_L = 3,

        DUT_bDPA_R = 4,

        DUT_System = 5,

        DUT_Display = 6,

        DUT_Display_L = 7,
        DUT_Display_R = 8,

    };

    struct SpotData
    {
        int id;
        double x;
        double y;
        double radius;
    };

    struct TxTyTz
    {
        double Tx;
        double Ty;
        double Tz;
    };

    typedef enum MTF_TYPE
    {
        CROSS = 0,

        SLANT = 1,

        SPOT = 2

    }ML_MTF_TYPE;


    typedef enum FREQ_UNIT
    {
        lp_mm = 0,

        lp_deg = 1

    }ML_FREQ_UNIT;


    typedef enum POINT_DIR
    {
        Vertical = 1,
        
        Horizontal = 0

    }ML_POINT_DIR, ML_DIR;


    typedef struct ML_Point
    {
        int x;

        int y;

    }ML_POINT;

    typedef struct ML_Point3D
    {
        int x;
        int y;
        int z;
    }; ML_Point3D;

    typedef struct ML_Point6D
    {
        int x; //x linear distance
        int y; //y linear distance
        int z; //z linear distance
        int dx;
        int dy;
        int dz;
    }ML_POINT6D;

    typedef struct ML_AxialAngle
    {
        double u;
        double v;
        double w;
    }; ML_AxialAngle;

    typedef struct ML_Point2f
    {
        float x;

        float y;

    }ML_POINT2F;

    typedef struct DUTData
    {
        ML_Point2f ref_left;
        ML_Point2f ref_right;
        ML_Point2f left;
        ML_Point2f right;
    };


    struct Nits2GL
    {
        CORE::Color color;
        double fNits2GL;
    };

    struct Point2f
    {
        float x;
        float y;
    };
    struct LineEquation
    {
        double A;
        double B;
        std::vector<Point2f> points;
    };

    typedef struct ML_Roi
    {
        int index;

        int width;

        int height;

        ML_POINT center;

        POINT_DIR dir;

    }ML_ROI, MLROI;


    typedef struct Resolution
    {
        int Height;
        int Width;

    }ML_RES, ML_RESOLUTION;


    typedef struct MLRECT
    {
        int top;
        int left;
        int buttom;
        int right;

    }ML_RECT;




    typedef enum ML_Pixel_Format
    {
        ML_MONO8 = 0,	// 8 bits per pixel
        ML_MONO12 = 1,	// 12 bits per pixel
        ML_MONO16,		// 16 bits per pixel
        ML_RGB24,		// RGB data format

    } ML_PIXEL_FORMAT, ML_PIXEL_TYPE;

    typedef struct ML_ModuleInfo
    {
        int ID;

        const char* name;

        const char* cameraname;

        const char* motionname;

        const char* systemname;

    }ML_ModuleInfo, ML_MODULE_INFO;


    enum class MLGlobalVariable
    {
        kHorizontal = 0,

        kVertical = 1,

        kARInfinity = 9999

    };

    typedef enum MLMotionType
    {
        kComm = 0,
        kElmo = 1,
    };

    typedef enum class MLMotionState
    {
        kMLStatusStationary = 4,

        kMLStatusIsMoving = 3,

        kMLStatusConnected = 2,

        /** @brief operate successfully */
        kMLStatusOk = 1,

        /** @brief operation failure */
        kMLStatusFailed = 0,

        kMLStatusDisConnected = -1,

        /** @brief unknown error */
        kMLStatusUnknow = -2,

    };

    /** @brief Status Code */
    typedef enum MLStatus
    {
        ML_MOTION_STATUS_START_CROSSCENTER = 19,

        ML_MOTION_STATUS_FINISH_CROSSCENTER = 18,

        ML_MOTION_STATUS_START_MTF = 17,

        ML_MOTION_STATUS_FINISH_MTF = 16,

        ML_MOTION_STATUS_START_INIT = 15,

        ML_MOTION_STATUS_FINISH_INIT = 14,

        ML_CAMERA_STATUS_FINISH_AE = 13,

        ML_CAMERA_STATUS_START_AE = 12,

        ML_STATUS_FOCUS_FINE = 11,

        ML_STATUS_FOCUS_ROUGH = 10,

        ML_STATUS_MODULE_CONNECTED = 9,

        ML_STATUS_MOTION_OPENED = 8,

        ML_STATUS_CAMERA_OPENED = 7,

        ML_STATUS_IN_AE = 6,

        ML_STATUS_STOPFOCUS = 5,

        ML_STATUS_IN_SETFOCUSING = 4,

        ML_STATUS_CONNECTTING = 3,

        ML_STATUS_IN_FOCUSING = 2,

        /** @brief operate successfully */
        ML_STATUS_OK = 1,

        /** @brief operation failure */
        ML_STATUS_FAILED = 0,

        ML_STATUS_NULL = -1,

        /** @brief unknown error */
        ML_STATUS_UNKNOW = -2,

        /** @brief CAMERA ERROR */
        ML_STATUS_CAMERA_ERROR = -3,

        ML_STATUS_MOTION_ERROR = -4,

        /** @brief  */
        ML_STATUS_INVALIED = -5,

        ML_STATUS_CAMERA_UNOPEN = -6,

        ML_STATUS_MOTION_UNOPEN = -7,

        ML_STATUS_PARAM_INVALIED = -8,

        ML_STATUS_MODULE_UNCONNECT = -9,

        ML_CAMERA_STATUS_DEVICE_OFFLINE = -10,

        ML_MOTION_STATUS_DEVICE_OFFLINE = -11,

        ML_STATUS_TIMEOUT = -12,

        ML_STATUS_UNOPEN = -13,

    }MLStatus;

    typedef enum
    {
        Param_MTFType,
        Param_FreqUnit,
        Param_Pixel,
        Param_FocalLength,
        Param_Interp,
    }MLMTFParam;


    typedef enum MLMotionEvent
    {
        kStateChanged,
        kPositionChanged
    };


    enum InstrumentStatus
    {
        INITIALIZING,
        DISCONNECTED,
        CONNECTED,
        CONNECTION_FAILED,
        DISCONNECTION_FAILED,
        MISMATCH,
        IDLE,
        BUSY,
        IN_ERROR
    };


    typedef enum
    {
        Param_ROI_Index,
        Param_Freq,
        Param_FocusMin,
        Param_FocusMax,
        Param_RoughStep,
        Param_FineRange,
        Param_FineStep,
        Param_WaitMillisecond,
        Param_Smooth,
        Param_Precision,
    }MLAutoFocusParam;

    typedef enum
    {
        SysParam_MTFType,
        SysParam_FreqUnit,
        SysParam_Pixel,
        SysParam_FocalLength,
        SysParam_Interp,
        SysParam_MTFChartXMax,
        SysParam_MTFChartXMin,

        SysParam_ROIIndex,
        SysParam_Freq,
        SysParam_RoughStep,
        SysParam_FineRange,
        SysParam_FineStep,
        SysParam_Stepms,
        SysParam_Smooth,
        SysParam_Precision,

    }MLSysParam;


    typedef struct MLMTF_Result
    {
        double angle = 0;
        std::vector<double> m_vESF_X;
        std::vector<double> m_vESF_Y;
        std::vector<double> m_vLSF_X;
        std::vector<double> m_vLSF_Y;
        std::vector<double> m_vMTF_X;
        std::vector<double> m_vMTF_Y;

    }MTF_Result;


    typedef struct ML_RoiData
    {
        MLROI roi;

        MLMTF_Result mtfresult;

    } MLROI_RESULT;


    struct MinMax {
        float min = 0;
        float max = 0;
        MinMax()
        {
        }
        MinMax(float minT, float maxT)
        {
            min = minT;
            max = maxT;
        }
    };

    enum FiducialType{
        CIRCLE = 1,
        CROSSHAIR,
        BMW,
        RECTANGLE
    };

    typedef int(*MLAutoFocusCallback)(MLStatus status, double progress, int* steparray, double* distance, double* data, int len, void* pInstance);

    //typedef int(*MLStreamCallback)(void* pContext, Mat Frame, int totalnum, int errornum);

    typedef int(*MLModuleNotifyCallback)(MLStatus status,int moduleID, void* pInstance);

    typedef void(*MLAutoExposureCallback)(int exposure_time, double gray, int current_step, int total_step, std::string msg);
}

#endif // !COMMON_ML_H_
