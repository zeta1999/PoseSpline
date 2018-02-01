#ifndef VECTORSPACESPLINE_H
#define VECTORSPACESPLINE_H

#include "pose-spline/Quaternion.hpp"
#include "splines/bspline.hpp"
#include "utility/Time.hpp"


namespace ze {

    class VectorSpaceSpline : public BSpline {

    public:
        VectorSpaceSpline(int spline_order);
        VectorSpaceSpline(int spline_order,double interval);

        virtual ~VectorSpaceSpline();
        void setTimeInterval(double timeInterval); // need virtual
        double getTimeInterval();  //  need virtual
        bool isTsEvaluable(double ts); // need vitual

        void addSample(double t, Eigen::Vector3d Q); // need vitual
        void initialSpline(std::vector<std::pair<double,Eigen::Vector3d>> Meas); // need vitual

        void initialSplineKnot(double t);

        void printKnots();

        inline size_t getControlPointNum(){
            return mControlPointsParameter.size();
        }

        inline double* getControlPoint(unsigned int i){
            return mControlPointsParameter.at(i);
        }
    private:
        void initialNewControlPoint(); // need pure vitual
        std::vector<double*> mControlPointsParameter;
        std::map<double, Eigen::Vector3d> mSampleValues;
        double mTimeInterval;

    };
}

#endif