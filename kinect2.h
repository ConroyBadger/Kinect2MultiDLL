#ifndef KINECT_H
#define KINECT_H

#include "Kinect.h"

#define COLOR_W (1920)
#define COLOR_H (1080)
#define COLOR_BPP (4)
#define COLOR_SIZE (COLOR_W * COLOR_H * COLOR_BPP)

#define DEPTH_W (512)
#define DEPTH_H (424)
#define DEPTH_BPP (2)

class Kinect2;

extern Kinect2 *kinect;

struct RGBAPixel {
  UINT8 *r,g,b,a;
};

struct BGRAPixel {
  UINT8 b,g,r,a;
};

struct Point2D {
  float x,y;
};

struct BodyData {
  UINT64 trackingID;
  BOOLEAN tracked;
  TrackingConfidence confidence;
  HandState leftHandState;
  HandState rightHandState;
  Joint joint[JointType_Count];
  ColorSpacePoint jointColorPt[JointType_Count];
};

typedef BodyData BodyDataArray[BODY_COUNT];

typedef BGRAPixel ColorBuffer[COLOR_W * COLOR_H];

class Kinect2
{
private:

public:
  IKinectSensor *sensor;

  IDepthFrameReader *depthFrameReader;
  IDepthFrame *depthFrame = 0;

  IInfraredFrameReader *irFrameReader;
  IInfraredFrame *irFrame = 0;

  IColorFrameReader *colorFrameReader;
  IColorFrame *colorFrame = 0;

  IBodyFrameReader *bodyFrameReader;
  IBodyFrame *bodyFrame = 0;

  ICoordinateMapper *coordinateMapper;

  IMultiSourceFrameReader *multiSourceFrameReader;

  IMultiSourceFrame *multiSourceFrame = NULL;

  UINT16 *depthData;
  UINT16 *irData;
  UINT8 *colorData;

  UINT8 *testData;

  ColorBuffer colorBuffer;

  int colorFrames;
  int irFrames;
  int depthFrames;

  bool ready;
  
  BodyData *bodyData;

  Kinect2();
  ~Kinect2();

  void initSDK();

  void initTestData();

  bool ableToInitDefaultSensor();

  bool ableToInitCoordinateMapper();

  bool ableToStartMultiFrame();
  bool ableToUpdateMultiFrame();
  void doneMultiFrame();

  bool ableToStartDepthStream();
  bool ableToUpdateDepth();
  void freeDepthFrame();

  bool ableToStartIRStream();
  bool ableToUpdateIR();
  void freeIRFrame();

  bool ableToStartColorStream();
  bool ableToUpdateColor();
  void freeColorFrame();

  bool ableToStartBodyStream();
  bool ableToUpdateBody();
  void doneBodyFrame();

  void setTrackedCount(int count);

  bool ableToProcessColorFrame(IColorFrame *colorFrame);
  bool ableToProcessBodyFrame(IBodyFrame *bodyFrame);

  void processBodies(IBody** ppBodies);

  void drawBody(const Joint* pJoints, Point2D *jointPoints);
  void drawHand(HandState handState, Point2D *handPosition);
  void drawBone(const Joint* pJoints, Point2D *jintPoints,
                JointType joint0, JointType joint1);
};

#endif // KINECT_H
