#pragma once

class Manifold;
class Collider;

typedef bool(*CollisionCallback)(Collider *a, Collider *b, float *dist);

extern CollisionCallback Dispatch[Shape::eCount][Shape::eCount];

bool RaytoRay(Collider *a, Collider *b, float *dist = NULL);
bool RaytoSphere(Collider *a, Collider *b, float *dist = NULL);
bool RaytoBox(Collider *a, Collider *b, float *dist = NULL);
bool RaytoCapsule(Collider *a, Collider *b, float *dist = NULL);
bool RaytoPlane(Collider *a, Collider *b, float *dist = NULL);
bool SpheretoRay(Collider *a, Collider *b, float *dist = NULL);
bool SpheretoSphere(Collider *a, Collider *b, float *dist = NULL);
bool SpheretoBox(Collider *a, Collider *b, float *dist = NULL);
bool SpheretoCapsule(Collider *a, Collider *b, float *dist = NULL);
bool SpheretoPlane(Collider *a, Collider *b, float *dist = NULL);
bool BoxtoRay(Collider *a, Collider *b, float *dist = NULL);
bool BoxtoSphere(Collider *a, Collider *b, float *dist = NULL);
bool BoxtoBox(Collider *a, Collider *b, float *dist = NULL);
bool BoxtoCapsule(Collider *a, Collider *b, float *dist = NULL);
bool BoxtoPlane(Collider *a, Collider *b, float *dist = NULL);
bool CapsuletoRay(Collider *a, Collider *b, float *dist = NULL);
bool CapsuletoSphere(Collider *a, Collider *b, float *dist = NULL);
bool CapsuletoBox(Collider *a, Collider *b, float *dist = NULL);
bool CapsuletoCapsule(Collider *a, Collider *b, float *dist = NULL);
bool CapsuletoPlane(Collider *a, Collider *b, float *dist = NULL);
bool PlanetoRay(Collider *a, Collider *b, float *dist = NULL);
bool PlanetoSphere(Collider *a, Collider *b, float *dist = NULL);
bool PlanetoBox(Collider *a, Collider *b, float *dist = NULL);
bool PlanetoCapsule(Collider *a, Collider *b, float *dist = NULL);
bool PlanetoPlane(Collider *a, Collider *b, float *dist = NULL);


//////////////////////////////////////////////////////////////////////////
// Picking
//////////////////////////////////////////////////////////////////////////