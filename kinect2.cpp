#include "kinect2.h"

Kinect2 *kinect = NULL;

template<class Interface>

inline void SafeRelease(Interface *& pInterfaceToRelease)
{
  if (pInterfaceToRelease != NULL) {
    pInterfaceToRelease->Release();
    pInterfaceToRelease = NULL;
  }
}

Kinect2::Kinect2()
{
  colorFrames = 0;
  irFrames = 0;
  depthFrames = 0;

  depthFrame = NULL;
  irFrame = NULL;
  colorFrame = NULL;

  testData = (UINT8 *) malloc(COLOR_SIZE);
  initTestData();

  ready = false;

  int size = BODY_COUNT * sizeof(BodyData);
  bodyData = (BodyData *) malloc(size);

  if (!this->ableToInitDefaultSensor()) return;
  if (!this->ableToInitCoordinateMapper()) return;

  int hr = sensor->Open();

  ready = SUCCEEDED(hr);
}

Kinect2::~Kinect2()
{
  if (sensor) sensor->Close();
  if (bodyData) free(bodyData);
  free(testData);
}

void Kinect2::initTestData()
{
  BGRAPixel *pixelPtr = (BGRAPixel *) testData;

  for (int y = 0; y < COLOR_H; y++) {
    UINT8 red = (UINT8) (255 * y / COLOR_H);
    for (int x = 0; x  < COLOR_W; x++) {
      pixelPtr->r = red;
      pixelPtr->g = 255;
      pixelPtr->b = 255;
      pixelPtr->a = 255;
      pixelPtr++;
    }
  }
}

bool Kinect2::ableToInitDefaultSensor()
{
  HRESULT hr;

  hr = GetDefaultKinectSensor(&sensor);
  if (FAILED(hr)) return false;
  if (!sensor) return false;

  return true;
}

bool Kinect2::ableToStartMultiFrame()
{
  DWORD fst;
  fst = FrameSourceTypes::FrameSourceTypes_Color |
        FrameSourceTypes::FrameSourceTypes_Body;

  int hr = sensor->OpenMultiSourceFrameReader(fst, &multiSourceFrameReader);

  return SUCCEEDED(hr);
}

bool Kinect2::ableToUpdateMultiFrame()
{
// try to get a multisource frame
  HRESULT hr = multiSourceFrameReader->AcquireLatestFrame(&multiSourceFrame);

// get the color frame
  if (SUCCEEDED(hr)) {
    IColorFrameReference* colorFrameReference = NULL;
    hr = multiSourceFrame->get_ColorFrameReference(&colorFrameReference);
    if (SUCCEEDED(hr)) {
      hr = colorFrameReference->AcquireFrame(&colorFrame);
      if (!ableToProcessColorFrame(colorFrame)) hr = -1;
    }
    SafeRelease(colorFrameReference);
  }

// get the body frame
  if (SUCCEEDED(hr)) {
    IBodyFrameReference *bodyFrameReference = NULL;
    hr = multiSourceFrame->get_BodyFrameReference(&bodyFrameReference);
    if (SUCCEEDED(hr)) {
      hr = bodyFrameReference->AcquireFrame(&bodyFrame);
      if (SUCCEEDED(hr)) {
        if (!ableToProcessBodyFrame(bodyFrame)) hr = -1;
      }
    }
    SafeRelease(bodyFrameReference);
  }
  return SUCCEEDED(hr);
}

void Kinect2::doneMultiFrame()
{
  SafeRelease(colorFrame);
  SafeRelease(bodyFrame);
  SafeRelease(multiSourceFrame);
}

bool Kinect2::ableToStartDepthStream()
{
// get the depth source
  IDepthFrameSource* depthFrameSource = NULL;
  HRESULT hr = sensor->get_DepthFrameSource(&depthFrameSource);

// open the reader
  if (SUCCEEDED(hr)) {
    hr = depthFrameSource->OpenReader(&depthFrameReader);
    //  SafeRelease(depthFrameSource);
    if (!SUCCEEDED(hr)) return false;
    if (!depthFrameReader) return false;
    return true;
  }
  return false;
}

bool Kinect2::ableToStartIRStream()
{
// get the IR source
  IInfraredFrameSource *irFrameSource = NULL;
  HRESULT hr = sensor->get_InfraredFrameSource(&irFrameSource);

// open the reader
  if (SUCCEEDED(hr)) {
    hr = irFrameSource->OpenReader(&irFrameReader);
    return SUCCEEDED(hr);
  }
  return false;
}

void Kinect2::initSDK()
{
}

/*inline void SafeRelease(Interface *& pInterfaceToRelease)
{
  if (pInterfaceToRelease != NULL) {
    pInterfaceToRelease->Release();
    pInterfaceToRelease = NULL;
  }
}*/

bool Kinect2::ableToUpdateDepth()
{
  depthFrame = NULL;
  depthData = NULL;

  UINT nBufferSize = 0;

  HRESULT hr = depthFrameReader->AcquireLatestFrame(&depthFrame);

  if (SUCCEEDED(hr)) {
    hr = depthFrame->AccessUnderlyingBuffer(&nBufferSize, &depthData);
  }

  return (SUCCEEDED(hr));
}

void Kinect2::freeDepthFrame()
{
  if (depthFrame) {
    depthFrame->Release();
    depthFrame = NULL;
  }
}

bool Kinect2::ableToUpdateIR()
{
  if (!irFrameReader) return false;


  HRESULT hr = irFrameReader->AcquireLatestFrame(&irFrame);

  if (SUCCEEDED(hr)) {
    INT64 nTime = 0;
    UINT nBufferSize = 0;

    hr = irFrame->get_RelativeTime(&nTime);

    if (SUCCEEDED(hr)) {
      hr = irFrame->AccessUnderlyingBuffer(&nBufferSize, &irData);
    }
  }

  return (SUCCEEDED(hr));
}

void Kinect2::freeIRFrame()
{
  if (irFrame) {
    irFrame->Release();
    irFrame = NULL;
  }
}

bool Kinect2::ableToStartColorStream()
{
// get the IR source
  IColorFrameSource *colorFrameSource = NULL;
  HRESULT hr = sensor->get_ColorFrameSource(&colorFrameSource);

// open the reader
  if (SUCCEEDED(hr)) {
    hr = colorFrameSource->OpenReader(&colorFrameReader);
    return SUCCEEDED(hr);
  }
  return false;
}

bool Kinect2::ableToProcessColorFrame(IColorFrame *colorFrame)
{
  UINT nBufferSize = 0;
  UINT8 *pBuffer = 0;
  ColorImageFormat imageFormat = ColorImageFormat_None;

  int hr = colorFrame->get_RawColorImageFormat(&imageFormat);

  if (FAILED(hr)) return false;

  if (imageFormat == ColorImageFormat_Bgra) {
    hr = colorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
  }
  else {
    int size = COLOR_W * COLOR_H * 4;
    hr = colorFrame->CopyConvertedFrameDataToArray(size, (UINT8 *) colorBuffer, ColorImageFormat_Bgra);
  }

  if (SUCCEEDED(hr)) {
    colorData = (UINT8 *) colorBuffer;
    return true;
  }
  return false;
}

bool Kinect2::ableToUpdateColor()
{
  if (!colorFrameReader) return false;

  HRESULT hr = colorFrameReader->AcquireLatestFrame(&colorFrame);

  if (SUCCEEDED(hr)) {
    if (!ableToProcessColorFrame(colorFrame)) hr = -1;
  }

  return SUCCEEDED(hr);
}

void Kinect2::freeColorFrame()
{
  if (colorFrame) {
    colorFrame->Release();
    colorFrame = NULL;
  }
}

bool Kinect2::ableToStartBodyStream()
{
// get the body source
  IBodyFrameSource *bodyFrameSource = NULL;
  HRESULT hr = sensor->get_BodyFrameSource(&bodyFrameSource);

// open the reader
  if (SUCCEEDED(hr)) {
    hr = bodyFrameSource->OpenReader(&bodyFrameReader);
  }

  SafeRelease(bodyFrameSource);

  return SUCCEEDED(hr);
}

void Kinect2::setTrackedCount(int count)
{
  for (int b = 0; b < BODY_COUNT; b++) {
    bodyData[b].tracked = (b < count);
  }
}

void Kinect2::processBodies(IBody** bodies)
{
  for (int b = 0; b < BODY_COUNT; b++) {
    IBody* body = bodies[b];

    if (!body) {
      bodyData[b].tracked = false;
      continue;
    }

    int rc = body->get_IsTracked(&bodyData[b].tracked);

    if (!SUCCEEDED(rc)) bodyData[b].tracked = false;

    if (!bodyData[b].tracked) continue;

// copy the hand states
    body->get_HandLeftState(&bodyData[b].leftHandState);
    body->get_HandRightState(&bodyData[b].rightHandState);

// copy the joints
    body->GetJoints(JointType_Count, bodyData[b].joint);

// get the position in the color camera
    for (int j = 0; j < JointType_Count; ++j) {
      coordinateMapper->MapCameraPointToColorSpace(bodyData[b].joint[j].Position,
                                                   &bodyData[b].jointColorPt[j]);
    }
  }
}

bool Kinect2::ableToProcessBodyFrame(IBodyFrame *bodyFrame)
{
  IBody* body[BODY_COUNT] = {0};

  int hr = bodyFrame->GetAndRefreshBodyData(BODY_COUNT, body);

  if (SUCCEEDED(hr)) {
    processBodies(body);
  }

// clean up
  for (int i = 0; i < BODY_COUNT; i++) {
    SafeRelease(body[i]);
  }

  return SUCCEEDED(hr);
}

bool Kinect2::ableToUpdateBody()
{
  if (!coordinateMapper) return false;

  if (!bodyFrameReader) return false;

  HRESULT hr = bodyFrameReader->AcquireLatestFrame(&bodyFrame);


  if (SUCCEEDED(hr)) {

    if (!ableToProcessBodyFrame(bodyFrame)) {
      hr = -1;
    }
  }

  return SUCCEEDED(hr);
}

void Kinect2::doneBodyFrame()
{
  SafeRelease(bodyFrame);
}

bool Kinect2::ableToInitCoordinateMapper()
{
  int rc = sensor->get_CoordinateMapper(&coordinateMapper);
  return SUCCEEDED(rc);
}
