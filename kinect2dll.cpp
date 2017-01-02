#include "kinect2dll.h"

Kinect2DLL::Kinect2DLL()
{
}

extern "C" __declspec(dllexport) int getVersion()
{
  return 9;
}

extern "C" __declspec(dllexport) bool ableToStartUp()
{
  kinect = new Kinect2();
  return kinect->ready;
}

extern "C" __declspec(dllexport) bool ableToStartDepth()
{
  if (!kinect) return false;

  return kinect->ableToStartDepthStream();
}

extern "C" __declspec(dllexport) UINT16* getDepthFrame()
{
  if (!kinect->ready) return NULL;

  if (kinect->ableToUpdateDepth()) {
    return kinect->depthData;
  }

  return NULL;
}

// this needs to be called even if getFrame() returns NULL as
// kinect->ableToUpdate() can fail even if a depthFrame is acquired
// kinect->freeDepthFrame checks for NULL so it's safe
extern "C" __declspec(dllexport) void doneDepthFrame()
{
  kinect->freeDepthFrame();
}

extern "C" __declspec(dllexport) bool ableToStartIR()
{
  if (!kinect) return false;

  return kinect->ableToStartIRStream();
}

extern "C" __declspec(dllexport) UINT16* getIRFrame()
{
  if (!kinect->ready) return NULL;

  if (kinect->ableToUpdateIR()) {
    return kinect->irData;
  }

  return NULL;
}

// this needs to be called even if getFrame() returns NULL as
// kinect->ableToUpdate() can fail even if a depthFrame is acquired
// kinect->freeDepthFrame checks for NULL so it's safe
extern "C" __declspec(dllexport) void doneIRFrame()
{
  kinect->freeIRFrame();
}

extern "C" __declspec(dllexport) bool ableToStartColor()
{
  if (!kinect) return false;

  return kinect->ableToStartColorStream();
}

extern "C" __declspec(dllexport) UINT8* getColorFrame()
{
  if (!kinect->ready) return NULL;

  if (kinect->ableToUpdateColor()) {
    return kinect->colorData;
  }

  return NULL;
}

extern "C" __declspec(dllexport) UINT8 *getColorData()
{
  return kinect->testData;
//  return kinect->colorData;

}

// this needs to be called even if getFrame() returns NULL as
// kinect->ableToUpdate() can fail even if a depthFrame is acquired
// kinect->freeDepthFrame checks for NULL so it's safe
extern "C" __declspec(dllexport) void doneColorFrame()
{
  kinect->freeColorFrame();
}

extern "C" __declspec(dllexport) bool ableToStartMultiFrame()
{
  return kinect->ableToStartMultiFrame();
}

extern "C" __declspec(dllexport) bool ableToUpdateMultiFrame()
{
  return kinect->ableToUpdateMultiFrame();
}

extern "C" __declspec(dllexport) void doneMultiFrame()
{
  return kinect->doneMultiFrame();
}


// body
extern "C" __declspec(dllexport) bool ableToStartBody()
{
  return kinect->ableToStartBodyStream();
}

extern "C" __declspec(dllexport) bool ableToUpdateBodyFrame() // void *data)
{
  return kinect->ableToUpdateBody();//data);
}

extern "C" __declspec(dllexport) void doneBodyFrame()
{
  return kinect->doneBodyFrame();
}

extern "C" __declspec(dllexport) BodyData *getBodyData()
{
  BodyData *data = kinect->bodyData;

  return data;
}

extern "C" __declspec(dllexport) void shutDown()
{
  delete kinect;
}
