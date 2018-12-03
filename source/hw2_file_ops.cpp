#include <cstdio>
#include <cstdlib>

#include "tinymath.h"
#include "hw2_file_ops.h"

extern std::vector<Camera> cameras;
int numberOfCameras = 0;

extern std::vector<Model> models;
int numberOfModels = 0;

extern std::vector<Color> colors;
int numberOfColors = 0;

extern std::vector<Translation> translations;
int numberOfTranslations = 0;

extern std::vector<Rotation> rotations;
int numberOfRotations = 0;

extern std::vector<Scaling> scalings;
int numberOfScalings = 0;

extern std::vector<tinymath::vec3> vertices;
int numberOfVertices = 0;

extern Color backgroundColor;
extern int backfaceCullingSetting;

extern Color **image;

/*
 * Reads camera file.
 */
void readCameraFile(char *camFileName) {
    FILE *fp;
    int i = 0;
    char line[80] = {};
    fp = fopen(camFileName, "r");

    if (fp == NULL) {
        printf("ERROR: Can't open %s\n", camFileName);
        exit(1);
    }

    /* read number of cameras */
    fscanf(fp, "%d", &numberOfCameras);

    for (i = 0; i < numberOfCameras; i++) {
        /* skip line "#Camera n" */
        Camera cam;
        fscanf(fp, "%s %d", line, &(cam.cameraId));
        fscanf(fp, "%lf %lf %lf", &(cam.pos.x), &(cam.pos.y), &(cam.pos.z));
        fscanf(fp, "%lf %lf %lf", &(cam.gaze.x), &(cam.gaze.y), &(cam.gaze.z));
        fscanf(fp, "%lf %lf %lf", &(cam.v.x), &(cam.v.y), &(cam.v.z));

        cam.gaze = tinymath::normalize(cam.gaze);
        cam.u = tinymath::cross(cam.gaze, cam.v);
        cam.u = tinymath::normalize(cam.u);

        cam.w.x = -cam.gaze.x;
        cam.w.y = -cam.gaze.y;
        cam.w.z = -cam.gaze.z;
        cam.v = tinymath::cross(cam.u, cam.gaze);
        cam.v = tinymath::normalize(cam.v);


        fscanf(fp, "%lf %lf %lf %lf", &(cam.l), &(cam.r), &(cam.b), &(cam.t));
        fscanf(fp, "%lf", &(cam.n));
        fscanf(fp, "%lf", &(cam.f));
        fscanf(fp, "%d %d", &(cam.sizeX), &(cam.sizeY));
        fscanf(fp, "%s", cam.outputFileName);
        
        cameras.push_back(cam);
    }
}

/*
 * Reads scene file.
 */
void readSceneFile(char *sceneFileName) {
    FILE *fp;
    char line[80];
    int i, j;
    char tmp[80];

    fp = fopen(sceneFileName, "r");

    if (fp == NULL) {
        printf("ERROR: Can't open %s\n", sceneFileName);
        exit(1);
    }

    /* read background color */
    fscanf(fp, "%lf %lf %lf", &(backgroundColor.r), &(backgroundColor.g), &(backgroundColor.b));

    /* read backface culling setting, enabled-disabled */
    fscanf(fp, "%d", &(backfaceCullingSetting));

    /* skip line "#Vertices" */
    fscanf(fp, "%s", line);

    /* read number of vertices */
    fscanf(fp, "%d", &numberOfVertices);
    numberOfColors = numberOfVertices;

    /* skip line "#Colors" */
    fscanf(fp, "%s", line);

    for (i = 1; i <= numberOfColors; i++) {
        Color color;
        fscanf(fp, "%lf %lf %lf", &(color.r), &(color.g), &(color.b));
        colors.push_back(color);
    }

    /* skip line "#Positions" */
    fscanf(fp, "%s", line);

    for (i = 1; i <= numberOfVertices; i++) {
        tinymath::vec3 vec;
        fscanf(fp, "%lf %lf %lf", &(vec.x), &(vec.y), &(vec.z));
        vec.colorId = i;
        vertices.push_back(vec);
    }

    /* skip line "#Translations" */
    fscanf(fp, "%s", line);

    /* read number of translations */
    fscanf(fp, "%d", &numberOfTranslations);

    for (i = 1; i <= numberOfTranslations; i++) {
        Translation t;
        fscanf(fp, "%lf %lf %lf", &(t.tx), &(t.ty), &(t.tz));
        translations.push_back(t);
    }

    /* skip line "#Scalings" */
    fscanf(fp, "%s", line);

    /* read number of scalings */
    fscanf(fp, "%d", &numberOfScalings);

    for (i = 1; i <= numberOfScalings; i++) {
        Scaling s;
        fscanf(fp, "%lf %lf %lf", &(s.sx), &(s.sy), &(s.sz));
        scalings.push_back(s);
    }

    /* skip line "#Rotations" */
    fscanf(fp, "%s", line);

    /* read number of rotations */
    fscanf(fp, "%d", &numberOfRotations);

    for (i = 1; i <= numberOfRotations; i++) {
        Rotation r;
        fscanf(fp, "%lf %lf %lf %lf", &(r.angle), &(r.ux), &(r.uy), &(r.uz));
        rotations.push_back(r);
    }

    /* skip line "#Models" */
    fscanf(fp, "%s", line);

    /* read number of models */
    fscanf(fp, "%d", &numberOfModels);

    for (i = 0; i < numberOfModels; i++) {
        Model m;
        /* read model id */
        fscanf(fp, "%d", &(m.modelId));

        /* read model type */
        fscanf(fp, "%d", &(m.type));

         /* read number of transformations */
        fscanf(fp, "%d", &(m.numberOfTransformations));

        for (j = 0; j < m.numberOfTransformations; j++) {
            fscanf(fp, "%s", tmp);
            m.transformationTypes[j] = tmp[0];
            fscanf(fp, "%d", &(m.transformationIDs[j]));
        }

        /* read number of triangles */
        fscanf(fp, "%d", &(m.numberOfTriangles));

        for (j = 0; j < m.numberOfTriangles; j++) {
            Triangle t;
            fscanf(fp, "%d %d %d", &(t.vertexIds[0]), &(t.vertexIds[1]), &(t.vertexIds[2]));
            m.triangles.push_back(t);
        }

        //Find vertice ids used in the model. store them
        for (auto & triangle : m.triangles) {
            for (const auto vertexId : triangle.vertexIds) {
                if (m.usedVertices.find(vertexId) == m.usedVertices.end()) {
                    m.usedVertices.insert(vertexId);
                }
            } 
        }
        
        models.push_back(m);
    }

    fclose(fp);

}

/*
 * If given value is less than 0, converts value to 0.
 * If given value is more than 255, converts value to 255.
 * Otherwise returns value itself.
 */
int clamp(double value) {
    if (value >= 255.0) return 255;
    if (value <= 0.0) return 0;
    return (int) value;
}

/*
 * Writes contents of image (Color**) into a PPM file.
 */
void writeImageToPPMFile(Camera camera) {
    FILE *outputFile;
    int i, j;

    outputFile = fopen(camera.outputFileName, "w");

    fprintf(outputFile, "P3\n");
    fprintf(outputFile, "# %s\n", camera.outputFileName);

    fprintf(outputFile, "%d %d\n", camera.sizeX, camera.sizeY);

    fprintf(outputFile, "255\n");

    for (j = camera.sizeY - 1; j >= 0; j--) {
        for (i = 0; i < camera.sizeX; i++) {
            fprintf(outputFile, "%d %d %d ", clamp(image[i][j].r), clamp(image[i][j].g), clamp(image[i][j].b));
        }
        fprintf(outputFile, "\n");
    }
    fclose(outputFile);
}


/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
	os_type == 1 		-> Ubuntu
	os_type == 2 		-> Windows
	os_type == other	-> No conversion
*/
void convertPPMToPNG(char *ppmFileName, int os_type) {
    char command[100];

    // call command on Ubuntu
    if (os_type == 1) {
        sprintf(command, "convert %s %s.png", ppmFileName, ppmFileName);
        system(command);
    }

        // call command on Windows
    else if (os_type == 2) {
        sprintf(command, "magick convert %s %s.png", ppmFileName, ppmFileName);
        system(command);
    }

        // default action - don't do conversion
    else {

    }
}
