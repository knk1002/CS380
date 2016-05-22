#ifndef AFFINE_H
#define AFFINE_H
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

/*
 * An affine matrix A can be factored as A = TL. You need to fill up two function named 'linearFact' and 'transFact'
 */

// TODO: Fill up linearFact function
// input: A (4 x 4 matrix)
// output: L (4 x 4 matrix)
glm::mat4 linearFact(glm::mat4 A)
{
	glm::mat4 L = A;
	//L[0][3] = 0;
	//L[1][3] = 0;
	//L[2][3] = 0;
	L[3][3] = 1;
	L[3][0] = 0;
	L[3][1] = 0;
	L[3][2] = 0;
	return L;
}

// TODO: Fill up transFact function
// input: A (4 x 4 matrix)
// output: T (4 x 4 matrix)
glm::mat4 transFact(glm::mat4 M)
{
	glm::mat4 T = glm::mat4(1.0f);
	T[3][0] = M[3][0];
	T[3][1] = M[3][1];
	T[3][2] = M[3][2];

	return T;
}
#endif
