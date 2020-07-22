#include "csUtility.h"

int randInt(int low, int high, bool inclusive)
{
    return low + (rand() % (high + inclusive));
}

/** \brief Gets the amount of digits in num.
 *
 * \param num - your number
 * \return the number of digits in num.
 */
int digits(int num)
{
    if (num < 0)
    {
        num *= -1;
    }

	return 1 + log10(num);
}

/** \brief free() with automatic NULL setting.
 * Use: x = freeThisMem(x);
 * \param x - memory address
 * \return NULL
 */
void* freeThisMem(void* x)
{
	free(x);
	return NULL;
}

char* removeNewline(char* stuff, char replacement, int maxLength)
{
    for(int i = maxLength - 1; i >= 0; i--)
    {
        if (i < 0)
            return stuff;
        if (stuff[i] == '\n' || stuff[i] == '\r')
        {
            stuff[i] = replacement;
        }
    }
    return stuff;
}


double getDistance(double x1, double y1, double x2, double y2)
{
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}

/*
/ ** \brief Checks an intersection between two lines.
 *
 * \param line1 - csDoubleLine representing line 1
 * \param line2 - csDoubleLine representint line 2
 * \return bool - true if line1 and line2 intersect, false otherwise
 * /
bool checkIntersection(cDoubleLine line1, cDoubleLine line2)
{  //TODO: make it work
    bool isCollided = false;
    const double accuracy = .0001;  //allowable floating point accuracy
    //bool vertLines[2] = {false, false};
    //double slopes[2] = {0, 0};
    //double intercepts[2] = {0, 0};
    double normals[2] = {0, 0};
    cDoubleLine lines[4];

    //sort from left to right
    lines[0].x1 = fmin(line1.x1, line1.x2);
    lines[0].x2 = fmax(line1.x1, line1.x2);
    lines[0].y1 = (lines[0].x1 == line1.x1) ? line1.y1 : line1.y2;
    lines[0].y2 = (lines[0].x2 == line1.x2) ? line1.x1 : line1.y2;

    lines[1].x1 = fmin(line2.x1, line2.x2);
    lines[1].x2 = fmax(line2.x1, line2.x2);
    lines[1].y1 = (lines[0].x1 == line2.x1) ? line2.y1 : line2.y2;
    lines[1].y2 = (lines[0].x2 == line2.x2) ? line2.x1 : line2.y2;

    //set normals
    for(int i = 0; i < 2; i++)
    {
        //slopes[i] = (lines[i].y2 - lines[i].y1) / (lines[i].x2 - lines[i].x1);  //set the slope
        normals[2 * i] = atan2(lines[i].y2 - lines[i].y1, lines[i].x2 - lines[i].x1);
        normals[2 * i + 1] = normals[2 * i] + degToRad(90.0);
    }

    / * set intercepts
    //y = mx + b so y - mx = b
    for(int i = 0; i < 2; i++)
    {
        if (!vertLines[i])
            intercepts[i] = lines[i].y1 - slopes[i] * lines[i].x1;  //set the y-intercept
        else
            intercepts[i] = lines[i].x1;  //set the x-intercept
    }
    // * /

    //print normals
    for(int i = 0; i < 4; i++)
        printf("Normals %d: %f\n", i, normals[i]);

    //SAT but with lines now
    for(int i = 0; i < 4; i++)
    {
        double mins[2] = {0, 0};
        double maxs[2] = {0, 0};
        for(int x = 0; x < 2; x++)
        {
            //sqrt(x^2 + y^2) * cos(theta);  //this is the projection of the selected corner onto a plane with some angle `theta` between them
            //essentially we get the magnitude of a vector from 0,0 to x,y (call it V) then multiply by the cosine of (the normal's angle - the angle of V)
            mins[x] = sqrt(pow(lines[x].x1, 2) + pow(lines[x].y1, 2)) * fabs(cos(fabs(normals[i] - atan2(lines[x].y1, lines[x].x1))));
            maxs[x] = mins[x];
            double newVal = sqrt(pow(lines[x].x2, 2) + pow(lines[x].y2, 2)) * fabs(cos(fabs(normals[i] - atan2(lines[x].y2, lines[x].x2))));

            if (newVal > maxs[x])
                maxs[x] = newVal;

            if (newVal < mins[x])
                mins[x] = newVal;

            //and we need to determine the minimum (closest to axis origin) and maximum (farther from axis origin)
            //by looping through each point, finding the projected point
        }

        //printf("%f or %f\n", min2 - max1, min1 - max2);
        if (mins[1] >= maxs[0] || mins[0] >= maxs[1])
        {
            isCollided = false;
            break;
        }
        else
        {
            isCollided = true;
        }
        //check for intersections of the two projected lines
        //  if not found, return false (because according to SAT if one gap in projections is found, there's a separating axis there)
        //  else continue
    }
    return isCollided;
    // *
    bool vertLine1 = false, vertLine2 = false;
    double slope1 = 0, slope2 = 0;
    double intercept1 = 0, intercept2 = 0;
    const double accuracy = .0001;  //allowable floating point accuracy

    double l1x1 = fmin(line1.x1, line1.x2), l1x2 = fmax(line1.x1, line1.x2);
    double l1y1 = fmin(line1.y1, line1.y2), l1y2 = fmax(line1.y1, line1.y2);

    double l2x1 = fmin(line2.x1, line2.x2), l2x2 = fmax(line2.x1, line2.x2);
    double l2y1 = fmin(line2.y1, line2.y2), l2y2 = fmax(line2.y1, line2.y2);

    if (fabs(l1x2 - l1x1) >= accuracy)  //if slope is not vertical
        slope1 = (l1y2 - l1y1) / (l1x2 - l1x1);  //set the slope
    else
        vertLine1 = true;  //set vertical flag

    if (fabs(l2x2 - l2x1) >= accuracy)
        slope2 = (l2y2 - l2y1) / (l2x2 - l2x1);
    else
        vertLine2 = true;

    //y = mx + b
    //y - mx = b
    if (!vertLine1)
        intercept1 = l1y1 - slope1 * l1x1;  //set the y-intercept
    else
        intercept1 = l1x1;  //set the x-intercept

    printf("-- 1.) Is vert: %s. y = %f x + %f\n", boolToString(vertLine1), slope1, intercept1);

    if (!vertLine2)
        intercept2 = l2y1 - slope2 * l2x1;
    else
        intercept2 = l2x1;

    printf("-- 2.) Is vert: %s. y = %f x + %f\n", boolToString(vertLine2), slope2, intercept2);

    // m1x + b1 = m2x + b2
    // (m1 - m2)x = b2 - b1
    double combinedSlopes = slope1 - slope2;
    double combinedIntercepts = intercept2 - intercept1;

    if (fabs(combinedSlopes) < accuracy)
    {  //if the saved slope values are equal
        if (vertLine1 == vertLine2)
        {  //if both are vertical lines or both are not
            if (fabs(combinedIntercepts) >= accuracy)  //if both have the same intercepts
                return false;
            else
                return true;
        }
        else
        {  //if one is vertical and one is not
            if (vertLine1)
            {  //if line 1 is the vertical
                double y = slope2 * intercept1 + intercept2;  //y = m2 * (x1-intercept) + b2
                if (y >= l1y1 && y <= l1y2)  //if y2 at x1 is between the two points of 1
                    return true;
                else
                    return false;
            }
            else
            {  //if line 2 is the vertical
                double y = slope1 * intercept2 + intercept1;  //y = m1 * (x2-intercept) + b1
                if (y >= l2y1 && y <= l2y2)  //if y1 at x2 is between the two points of 2
                    return true;
                else
                    return false;
            }
        }
    }

    //x = (b2 - b1) / (m1 - m2)
    double xCollided = combinedIntercepts / combinedSlopes;

    //m1xC + b1 = m2xC + b2 ?
    return fabs((slope1 * xCollided + intercept1) - (slope2 * xCollided + intercept2)) < accuracy;
    // * /
}
//*/
