#include <iostream>
#include <time.h>
#include <xmmintrin.h>
#include <iomanip>
#include <vector>


const unsigned int NUM_DISTINCT_BUFFERS = 16;

template<class VectorElementType>
inline double normalize2dArrayOfVectors(const unsigned int numIterations, const unsigned int numVectors, const unsigned int vectorSize) {
	const VectorElementType normalizedSum = 1.0; // target sum of all values per vector; pre-convert to either double or float representation of 1.0

	// each iteration will use it's own buffer, so we're allocating numVectors*numIterations length of data
	std::vector<VectorElementType **> iterationBuffers;
	iterationBuffers.resize(NUM_DISTINCT_BUFFERS);
	for (std::vector<VectorElementType **>::iterator iterationBufferPtr = iterationBuffers.begin(); iterationBufferPtr != iterationBuffers.end(); iterationBufferPtr++) {
		*iterationBufferPtr = new VectorElementType*[numVectors];
		for (unsigned int i = 0; i < numVectors; i++) {
			VectorElementType **vectorBuffer = *iterationBufferPtr;
			vectorBuffer[i] = new VectorElementType[vectorSize];
			for (unsigned int j = 0; j < vectorSize; j++) {
				vectorBuffer[i][j] = 0.22;
			}
		}
	}




	clock_t begin = clock();
	for (unsigned int iteration = 0; iteration < numIterations; iteration++) {
		// iterate through all influences
		for (VectorElementType **vertexWeightsPtr = iterationBuffers[iteration % NUM_DISTINCT_BUFFERS], **end= vertexWeightsPtr+numVectors; vertexWeightsPtr != end; vertexWeightsPtr++) {

			VectorElementType * const vertexWeights = *vertexWeightsPtr;
			VectorElementType sum = 0;
			for (unsigned int i = 0; i < vectorSize; i++) {
				sum += vertexWeights[i];
			}

			VectorElementType koef = normalizedSum / sum;

			for (unsigned int i = 0; i < vectorSize; i++) {
				vertexWeights[i] *= koef;
			}
		}

	}
	double result = (clock() - begin) / (1.0*CLOCKS_PER_SEC);

	// to hell with releasing the memory of this mess.
	for (std::vector<VectorElementType **>::iterator iterationBufferPtr = iterationBuffers.begin(); iterationBufferPtr != iterationBuffers.end(); iterationBufferPtr++) {
		VectorElementType **vectorBuffer = *iterationBufferPtr;
		for (unsigned int i = 0; i < numVectors; i++) {
			delete [] vectorBuffer[i];
		}
		delete [] vectorBuffer;
	}

	return result;
}

template<class VectorElementType>
inline double normalizeArrayOfVectors(const unsigned int numIterations, const unsigned int numVectors, const unsigned int vectorSize) {
	const VectorElementType normalizedSum = 1.0; // target sum of all values per vector; pre-convert to either double or float representation of 1.0

	std::vector<VectorElementType *> iterationBuffers;
	iterationBuffers.resize(NUM_DISTINCT_BUFFERS);
	for (std::vector<VectorElementType *>::iterator iterationBufferPtr = iterationBuffers.begin(); iterationBufferPtr != iterationBuffers.end(); iterationBufferPtr++) {
		*iterationBufferPtr = new VectorElementType[numVectors*vectorSize];
		for (unsigned int i = 0; i<numVectors*vectorSize; i++)
			(*iterationBufferPtr)[i] = 0.22f;
	}

	// a buffer for all vectors, where [0..vectorSize-1] elements are for first vector, [vectorSize..2*vectorSize-1] second, etc
	

	// set weights on the buffer to some value - we don't care about the actual value here.

	clock_t begin = clock();
	for (unsigned int iteration = 0; iteration < numIterations;iteration++) {

		// iterate through all influences
		for (VectorElementType *vertexWeights = iterationBuffers[iteration % NUM_DISTINCT_BUFFERS], *end=vertexWeights+numVectors*vectorSize; vertexWeights != end; vertexWeights += vectorSize) {
			VectorElementType sum = 0;
			for (unsigned int i = 0; i < vectorSize; i++) {
				sum += vertexWeights[i];
			}

			VectorElementType koef = normalizedSum / sum;

			for (unsigned int i = 0; i < vectorSize; i++) {
				vertexWeights[i] *= koef;
			}
		}
	}

	double result = (clock() - begin) / (1.0*CLOCKS_PER_SEC);
	for (std::vector<VectorElementType *>::iterator iterationBufferPtr = iterationBuffers.begin(); iterationBufferPtr != iterationBuffers.end(); iterationBufferPtr++) {
		delete[](*iterationBufferPtr);
	}
	return result;
}


typedef double (* NormalizeFunc)(const unsigned int numIterations, const unsigned int numVectors, const unsigned int vectorSize);

inline void compareImplementations(const char * const algorithmName, NormalizeFunc floatFunc, NormalizeFunc doubleFunc, const unsigned int numIterations, const unsigned int numVectors, const unsigned int vectorSize) {
	std::cout << algorithmName << ",";
	std::cout << vectorSize << ", ";
	std::cout << numVectors << ",";
	std::cout << numIterations << ",";

	double floatResult = floatFunc(numIterations, numVectors, vectorSize);
	double doubleResult = doubleFunc(numIterations, numVectors, vectorSize);
	double diff = doubleResult - floatResult;
	diff = (diff / floatResult) * 100;

	std::cout << floatResult << ",";
	std::cout << doubleResult << ",";
	std::cout << std::endl;
}

int main() {
	std::cout << "algorithm name,vector size,num vectors,num iterations,float time,double time"<<std::endl;
	for (unsigned int verts = 100000, iterations = 100; verts <= 100000; verts *= 10, iterations /= 10) {
		for (unsigned int influences = 1; influences <= 64; influences++) {
			compareImplementations("single buffer    ", normalizeArrayOfVectors<float>, normalizeArrayOfVectors<double>, iterations, verts, influences);
			compareImplementations("buffer of buffers", normalize2dArrayOfVectors<float>, normalize2dArrayOfVectors<double>, iterations, verts, influences);
		}
	}
	return 0;
}