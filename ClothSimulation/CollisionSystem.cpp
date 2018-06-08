#include "CollisionSystem.h"

#include "ClothComponent.h"
#include "SpringConstraint.h"
#include "Entity.h"
#include "Scene.h"
#include "GLUtils.h"
#include "VertexFormat.h"
#include "Mesh.h"
#include "Clock.h"
#include "GLPrimitives.h"

CollisionSystem::CollisionSystem(Scene& scene)
	: System(scene)
{
}


CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::update()
{
	for (size_t i = 0; i < m_scene.getEntityCount(); ++i) {
		Entity& clothEntity = m_scene.getEntity(i);
		if (clothEntity.hasComponents(COMPONENT_CLOTH)) {
			for (size_t j = 0; j < m_scene.getEntityCount(); ++j) {
				Entity& shapeEntity = m_scene.getEntity(j);
				if (shapeEntity.hasComponents(COMPONENT_SPHERE_COLLISION)) {
					SphereCollision(clothEntity, shapeEntity);
				}
				else if (shapeEntity.hasComponents(COMPONENT_PYRAMID_COLLISION)) {
					PyramidCollision(clothEntity, shapeEntity);
				}
			}
		}
	}
}

void CollisionSystem::beginFrame()
{
}

void CollisionSystem::endFrame()
{
}

void CollisionSystem::SphereCollision(Entity & clothEntity, Entity & sphereEntity)
{
	for (PointMass& pointMass : clothEntity.cloth.pointMasses) {
		glm::vec3 displacement = pointMass.getPosition() - sphereEntity.transform.position;
		if (glm::length(displacement) < sphereEntity.sphereCollision.radius + 0.04f) {
			glm::vec3 moveDirection = glm::normalize(displacement);
			pointMass.setPosition(sphereEntity.transform.position + (moveDirection * (sphereEntity.sphereCollision.radius + 0.04f)));
		}
	}
}

void CollisionSystem::PyramidCollision(Entity & clothEntity, Entity & pyramidEntity)
{
	auto pyramidPoints = pyramidEntity.pyramidCollision.GetPoints(pyramidEntity.transform);
	auto pyramidIndicies = GLPrimitives::getPyramidIndices();
	auto pointMasses = clothEntity.cloth.pointMasses;
	glm::vec3 pyramidScale = pyramidEntity.transform.scale;
	auto& triIndices = clothEntity.cloth.triIndices;

	// Loop through cloth
	for (size_t i = 0; i < triIndices.size(); i += 3){
		float pointTriangleDistance = glm::length(pointMasses.at(triIndices.at(i)).getPosition() - pyramidEntity.transform.position);
		if (pointTriangleDistance < glm::length(pyramidEntity.transform.scale)) {															// Works due to the triangle being 1 unit size
			// Loop through pyramid
			for (size_t j = 0; j < pyramidIndicies.size(); j += 3) {
				// Triangle 1
				std::vector<glm::vec3> triangle1 = {};
				triangle1.push_back(pointMasses.at(triIndices.at(i)).getPosition());
				triangle1.push_back(pointMasses.at(triIndices.at(i + 1)).getPosition());
				triangle1.push_back(pointMasses.at(triIndices.at(i + 2)).getPosition());
				// Triangle 2
				std::vector<glm::vec3> triangle2 = {};
				triangle2.push_back(pyramidPoints.at(pyramidIndicies.at(j)));
				triangle2.push_back(pyramidPoints.at(pyramidIndicies.at(j + 1)));
				triangle2.push_back(pyramidPoints.at(pyramidIndicies.at(j + 2)));

				int coplanar = 0;
				glm::vec3 intersect1;
				glm::vec3 intersect2;
				int intersecting = TriangleTriangleIntersection(triangle1, triangle2, coplanar, intersect1, intersect2);

				if (intersecting && ! coplanar) {
					glm::vec3 penetratingPoint;
					glm::vec3 pyramidPosition = pyramidEntity.transform.position;
					float point0dist = glm::distance(pyramidPosition, triangle1.at(0));
					float point1dist = glm::distance(pyramidPosition, triangle1.at(1));
					float point2dist = glm::distance(pyramidPosition, triangle1.at(2));

					/*clothEntity.cloth.pointMasses.at(triIndices.at(i)).addOffset((((intersect1 + intersect2) / 2.0f) - pyramidPosition) * 0.01f);
					clothEntity.cloth.pointMasses.at(triIndices.at(i + 1)).addOffset((((intersect1 + intersect2) / 2.0f) - pyramidPosition) * 0.01f);
					clothEntity.cloth.pointMasses.at(triIndices.at(i + 2)).addOffset((((intersect1 + intersect2) / 2.0f) - pyramidPosition) * 0.01f);*/

					// Gets the closest and 2nd closest point to the centre of the triangle
					if (point0dist < (glm::min(point1dist, point2dist))){
						penetratingPoint = triangle1.at(0);
					}
					else if (point1dist < point2dist) {
						penetratingPoint = triangle1.at(1);
					}
					else {
						penetratingPoint = triangle1.at(2);
					}

					float distance;
					glm::vec3 pointOnLine;
					glm::vec3 triangleOffset;

					PointLineDistance(penetratingPoint, intersect1, intersect2, distance, pointOnLine);
					triangleOffset = glm::normalize(pointOnLine - penetratingPoint) * distance;

					/*clothEntity.cloth.pointMasses.at(triIndices.at(i)).setPosition(clothEntity.cloth.pointMasses.at(triIndices.at(i)).getPosition() + triangleOffset);
					clothEntity.cloth.pointMasses.at(triIndices.at(i + 1)).setPosition(clothEntity.cloth.pointMasses.at(triIndices.at(i + 1)).getPosition() + triangleOffset);
					clothEntity.cloth.pointMasses.at(triIndices.at(i + 2)).setPosition(clothEntity.cloth.pointMasses.at(triIndices.at(i + 2)).getPosition() + triangleOffset);*/
					clothEntity.cloth.pointMasses.at(triIndices.at(i)).addOffset(triangleOffset);
					clothEntity.cloth.pointMasses.at(triIndices.at(i + 1)).addOffset(triangleOffset);
					clothEntity.cloth.pointMasses.at(triIndices.at(i + 2)).addOffset(triangleOffset);
				}
			}
		}
	}
}

void CollisionSystem::PointLineDistance(glm::vec3 point, glm::vec3 lineSeg1, glm::vec3 lineSeg2, float & distance, glm::vec3 & pointOnLine)
{
	float t = -((glm::dot((lineSeg1 - point), (lineSeg2 - lineSeg1))) / glm::pow(glm::length(lineSeg2 - lineSeg1), 2));
	pointOnLine = lineSeg1 + (lineSeg2 - lineSeg1) * t;
	distance = glm::length(pointOnLine - point);
}

void isect2(glm::vec3 VTX0, glm::vec3 VTX1, glm::vec3 VTX2, float VV0, float VV1, float VV2,
	float D0, float D1, float D2, float& isect0, float& isect1, glm::vec3& isectpoint0, glm::vec3& isectpoint1)
{
	float tmp = D0 / (D0 - D1);
	glm::vec3 diff;
	isect0 = VV0 + (VV1 - VV0)*tmp;
	diff =VTX1 - VTX0;
	diff = diff * tmp;
	isectpoint0 = diff + VTX0;
	tmp = D0 / (D0 - D2);
	isect1 = VV0 + (VV2 - VV0)*tmp;
	diff = VTX2 - VTX0;
	diff = diff * tmp;
	isectpoint1 = VTX0 + diff;
}

int compute_intervals_isectline(glm::vec3 VERT0, glm::vec3 VERT1, glm::vec3 VERT2,
	float VV0, float VV1, float VV2, float D0, float D1, float D2,
	float D0D1, float D0D2, float& isect0, float& isect1,
	glm::vec3& isectpoint0, glm::vec3& isectpoint1)
{
	if (D0D1>0.0f)
	{
		/* here we know that D0D2<=0.0 */
		/* that is D0, D1 are on the same side, D2 on the other or on the plane */
		isect2(VERT2, VERT0, VERT1, VV2, VV0, VV1, D2, D0, D1, isect0, isect1, isectpoint0, isectpoint1);
	}
	else if (D0D2>0.0f)
	{
		/* here we know that d0d1<=0.0 */
		isect2(VERT1, VERT0, VERT2, VV1, VV0, VV2, D1, D0, D2, isect0, isect1, isectpoint0, isectpoint1);
	}
	else if (D1*D2>0.0f || D0 != 0.0f)
	{
		/* here we know that d0d1<=0.0 or that D0!=0.0 */
		isect2(VERT0, VERT1, VERT2, VV0, VV1, VV2, D0, D1, D2, isect0, isect1, isectpoint0, isectpoint1);
	}
	else if (D1 != 0.0f)
	{
		isect2(VERT1, VERT0, VERT2, VV1, VV0, VV2, D1, D0, D2, isect0, isect1, isectpoint0, isectpoint1);
	}
	else if (D2 != 0.0f)
	{
		isect2(VERT2, VERT0, VERT1, VV2, VV0, VV1, D2, D0, D1, isect0, isect1, isectpoint0, isectpoint1);
	}
	else
	{
		/* triangles are coplanar */
		return 1;
	}
	return 0;
}
#define EDGE_EDGE_TEST(V0,U0,U1)                      \
  Bx=U0[i0]-U1[i0];                                   \
  By=U0[i1]-U1[i1];                                   \
  Cx=V0[i0]-U0[i0];                                   \
  Cy=V0[i1]-U0[i1];                                   \
  f=Ay*Bx-Ax*By;                                      \
  d=By*Cx-Bx*Cy;                                      \
  if((f>0 && d>=0 && d<=f) || (f<0 && d<=0 && d>=f))  \
  {                                                   \
    e=Ax*Cy-Ay*Cx;                                    \
    if(f>0)                                           \
    {                                                 \
      if(e>=0 && e<=f) return 1;                      \
    }                                                 \
    else                                              \
    {                                                 \
      if(e<=0 && e>=f) return 1;                      \
    }                                                 \
  }                         
#define EDGE_AGAINST_TRI_EDGES(V0,V1,U0,U1,U2) \
{                                              \
  float Ax,Ay,Bx,By,Cx,Cy,e,d,f;               \
  Ax=V1[i0]-V0[i0];                            \
  Ay=V1[i1]-V0[i1];                            \
  /* test edge U0,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U0,U1);                    \
  /* test edge U1,U2 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U1,U2);                    \
  /* test edge U2,U1 against V0,V1 */          \
  EDGE_EDGE_TEST(V0,U2,U0);                    \
}
#define POINT_IN_TRI(V0,U0,U1,U2)           \
{                                           \
  float a,b,c,d0,d1,d2;                     \
  /* is T1 completly inside T2? */          \
  /* check if V0 is inside tri(U0,U1,U2) */ \
  a=U1[i1]-U0[i1];                          \
  b=-(U1[i0]-U0[i0]);                       \
  c=-a*U0[i0]-b*U0[i1];                     \
  d0=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U2[i1]-U1[i1];                          \
  b=-(U2[i0]-U1[i0]);                       \
  c=-a*U1[i0]-b*U1[i1];                     \
  d1=a*V0[i0]+b*V0[i1]+c;                   \
                                            \
  a=U0[i1]-U2[i1];                          \
  b=-(U0[i0]-U2[i0]);                       \
  c=-a*U2[i0]-b*U2[i1];                     \
  d2=a*V0[i0]+b*V0[i1]+c;                   \
  if(d0*d1>0.0)                             \
  {                                         \
    if(d0*d2>0.0) return 1;                 \
  }                                         \
}
#define SORT2(a,b,smallest)       \
             if(a>b)       \
             {             \
               float c;    \
               c=a;        \
               a=b;        \
               b=c;        \
               smallest=1; \
             }             \
             else smallest=0;
int coplanar_tri_tri(glm::vec3 N, glm::vec3 V0, glm::vec3 V1, glm::vec3 V2,
	glm::vec3 U0, glm::vec3 U1, glm::vec3 U2)
{
	glm::vec3 A;
	short i0, i1;
	/* first project onto an axis-aligned plane, that maximizes the area */
	/* of the triangles, compute indices: i0,i1. */
	A[0] = glm::abs(N[0]);
	A[1] = glm::abs(N[1]);
	A[2] = glm::abs(N[2]);
	if (A[0]>A[1])
	{
		if (A[0]>A[2])
		{
			i0 = 1;      /* A[0] is greatest */
			i1 = 2;
		}
		else
		{
			i0 = 0;      /* A[2] is greatest */
			i1 = 1;
		}
	}
	else   /* A[0]<=A[1] */
	{
		if (A[2]>A[1])
		{
			i0 = 0;      /* A[2] is greatest */
			i1 = 1;
		}
		else
		{
			i0 = 0;      /* A[1] is greatest */
			i1 = 2;
		}
	}

	/* test all edges of triangle 1 against the edges of triangle 2 */
	EDGE_AGAINST_TRI_EDGES(V0, V1, U0, U1, U2);
	EDGE_AGAINST_TRI_EDGES(V1, V2, U0, U1, U2);
	EDGE_AGAINST_TRI_EDGES(V2, V0, U0, U1, U2);

	/* finally, test if tri1 is totally contained in tri2 or vice versa */
	POINT_IN_TRI(V0, U0, U1, U2);
	POINT_IN_TRI(U0, V0, V1, V2);

	return 0;
}

int CollisionSystem::TriangleTriangleIntersection(std::vector<glm::vec3>& T1, std::vector<glm::vec3>& T2, int& coplanar,
	glm::vec3& isectpt1, glm::vec3& isectpt2)
{
	glm::vec3 E1, E2, N1, N2;
	float d1, d2;
	float du0 = 0.0, du1 = 0.0, du2 = 0.0, dv0 = 0.0, dv1 = 0.0, dv2 = 0.0;
	glm::vec3 D;
	glm::vec2 isect1, isect2;
	glm::vec3 isectpointA1, isectpointA2;
	glm::vec3 isectpointB1, isectpointB2;
	float du0du1, du0du2, dv0dv1, dv0dv2;
	int index;
	float vp0, vp1, vp2;
	float up0, up1, up2;
	float b, c, max;
	float tmp; 
	glm::vec3 diff;
	int smallest1, smallest2;
	float epsilon = 0.000001;

	// Compute plane equation for triangle 1
	E1 = T1[1] - T1[0];
	E2 = T1[2] - T1[0];
	N1 = glm::cross(E1, E2);
	d1 = -glm::dot(N1, T1[0]);

	// Put triangle 2 into plane equationg to compute signed distance to plane
	du0 = glm::dot(N1, T2[0]) + d1;
	du1 = glm::dot(N1, T2[1]) + d1;
	du2 = glm::dot(N1, T2[2]) + d1;

	if (glm::abs(du0)<epsilon) du0 = 0.0;
	if (glm::abs(du1)<epsilon) du1 = 0.0;
	if (glm::abs(du2)<epsilon) du2 = 0.0;

	du0du1 = du0*du1;
	du0du2 = du0*du2;

	if (du0du1>0.0f && du0du2>0.0f)
		return 0;

	// Compute plane equation for triangle 2
	E1 = T2[1] - T2[0];
	E2 = T2[2] - T2[0];
	N2 = glm::cross(E1, E2);
	d2 = -glm::dot(N2, T2[0]);

	// Put triangle 1 into plane equationg to compute signed distance to plane
	dv0 = glm::dot(N2, T1[0]) + d2;
	dv1 = glm::dot(N2, T1[1]) + d2;
	dv2 = glm::dot(N2, T1[2]) + d2;

	if (glm::abs(dv0)<epsilon) dv0 = 0.0;
	if (glm::abs(dv1)<epsilon) dv1 = 0.0;
	if (glm::abs(dv2)<epsilon) dv2 = 0.0;

	dv0dv1 = dv0*dv1;
	dv0dv2 = dv0*dv2;

	if (dv0dv1>0.0f && dv0dv2>0.0f)
		return 0;

	// Compute direction of intersection line
	D = glm::cross(N1, N2);

	// Compute and index to the largest component of D
	max = glm::abs(D[0]);
	index = 0;
	b = glm::abs(D[1]);
	c = glm::abs(D[2]);
	if (b>max) max = b, index = 1;
	if (c>max) max = c, index = 2;

	// This is the simplified projection onto L
	vp0 = T1[0][index];
	vp1 = T1[1][index];
	vp2 = T1[2][index];

	up0 = T2[0][index];
	up1 = T2[1][index];
	up2 = T2[2][index];

	// Compute interval for triangle 1
	coplanar = compute_intervals_isectline(T1[0], T1[1], T1[2], vp0, vp1, vp2, dv0, dv1, dv2,
		dv0dv1, dv0dv2, isect1[0], isect1[1], isectpointA1, isectpointA2);

	if (coplanar) return coplanar_tri_tri(N1, T1[0], T1[1], T1[2], T2[0], T2[1], T2[2]);

	// Compute interval for triangle 2
	compute_intervals_isectline(T2[0], T2[1], T2[2], up0, up1, up2, du0, du1, du2,
		du0du1, du0du2, isect2[0], isect2[1], isectpointB1, isectpointB2);

	SORT2(isect1[0], isect1[1], smallest1);
	SORT2(isect2[0], isect2[1], smallest2);

	if (isect1[1]<isect2[0] || isect2[1]<isect1[0]) return 0;

	// Now we know there is an intersection
	if (isect2[0]<isect1[0])
	{
		if (smallest1 == 0) { isectpt1 = isectpointA1; }
		else { isectpt1 = isectpointA2; }

		if (isect2[1]<isect1[1])
		{
			if (smallest2 == 0) { isectpt2 = isectpointB2; }
			else { isectpt2 = isectpointB1; }
		}
		else
		{
			if (smallest1 == 0) { isectpt2 = isectpointA2; }
			else { isectpt2 = isectpointA1; }
		}
	}
	else
	{
		if (smallest2 == 0) { isectpt1 = isectpointB1; }
		else { isectpt1 = isectpointB2; }

		if (isect2[1]>isect1[1])
		{
			if (smallest1 == 0) { isectpt2 = isectpointA2; }
			else { isectpt2 = isectpointA1; }
		}
		else
		{
			if (smallest2 == 0) { isectpt2 = isectpointB2; }
			else { isectpt2 = isectpointB1; }
		}
	}
	return 1;
}
