#ifndef _TRACKVIS_H_
#define _TRACKVIS_H_

#include <RcppEigen.h>

#include "Streamline.h"
#include "DataSource.h"
#include "BinaryStream.h"

// Base class for Trackvis readers: provides common functionality
class TrackvisDataSource : public DataSource<Streamline>
{
protected:
    std::ifstream fileStream;
    BinaryInputStream binaryStream;
    int nScalars, nProperties, seedProperty;
    size_t totalStreamlines, currentStreamline;
    Eigen::Array3f voxelDims;
    
    void readStreamline (Streamline &data);
    
public:
    TrackvisDataSource ()
    {
        binaryStream.attach(&fileStream);
    }
    
    TrackvisDataSource (const std::string &fileStem)
    {
        binaryStream.attach(&fileStream);
        attach(fileStem);
    }
    
    virtual ~TrackvisDataSource ()
    {
        binaryStream.detach();
        if (fileStream.is_open())
            fileStream.close();
    }
    
    virtual void attach (const std::string &fileStem);
};

// Basic Trackvis reader: read all streamlines, including seed property
class BasicTrackvisDataSource : public TrackvisDataSource
{
public:
    bool more () const { return (currentStreamline < totalStreamlines); }
    void get (Streamline &data) { readStreamline(data); }
};

// Labelled Trackvis reader: also read auxiliary file containing label info
class LabelledTrackvisDataSource : public TrackvisDataSource
{
protected:
    std::ifstream auxFileStream;
    BinaryInputStream auxBinaryStream;
    std::map<int,std::string> labelDictionary;
    
public:
    LabelledTrackvisDataSource ()
    {
        auxBinaryStream.attach(&auxFileStream);
    }
    
    LabelledTrackvisDataSource (const std::string &fileStem)
    {
        auxBinaryStream.attach(&auxFileStream);
        attach(fileStem);
    }
    
    ~LabelledTrackvisDataSource ()
    {
        auxBinaryStream.detach();
        if (auxFileStream.is_open())
            auxFileStream.close();
    }
    
    void attach (const std::string &fileStem);
    bool more () const { return (currentStreamline < totalStreamlines); }
    void get (Streamline &data);
};

// Median Trackvis reader: construct and return median streamline only
class MedianTrackvisDataSource : public TrackvisDataSource
{
protected:
    bool read;
    double quantile;
    
public:
    MedianTrackvisDataSource ()
        : read(false) {}
    
    MedianTrackvisDataSource (const std::string &fileStem, const double quantile = 0.99)
        : TrackvisDataSource(fileStem), quantile(quantile), read(false) {}
    
    bool more () { return (!read); }
    void get (Streamline &data);
};

class TrackvisDataSink : public DataSink<Streamline>
{
protected:
    std::ofstream fileStream;
    BinaryOutputStream binaryStream;
    size_t totalStreamlines;
    Eigen::Array3f voxelDims;
    
    void writeStreamline (const Streamline &data);
    
public:
    static std::map<int,char> orientationCodeMap;
    
    static std::map<int,char> createOrientationCodeMap ()
    {
        std::map<int,char> map;
        map[NIFTI_L2R] = 'R';
        map[NIFTI_R2L] = 'L';
        map[NIFTI_P2A] = 'A';
        map[NIFTI_A2P] = 'P';
        map[NIFTI_I2S] = 'S';
        map[NIFTI_S2I] = 'I';
        return map;
    }
    
    TrackvisDataSink ()
    {
        binaryStream.attach(&fileStream);
        binaryStream.swapEndianness(false);
    }
    
    TrackvisDataSink (const std::string &fileStem, const NiftiImage &image)
    {
        binaryStream.attach(&fileStream);
        binaryStream.swapEndianness(false);
        attach(fileStem, image);
    }
    
    virtual ~TrackvisDataSink ()
    {
        binaryStream.detach();
        if (fileStream.is_open())
            fileStream.close();
    }
    
    virtual void attach (const std::string &fileStem, const NiftiImage &image);
    void setup (const size_type &count, const_iterator begin, const_iterator end);
    void done ();
};

class BasicTrackvisDataSink : public TrackvisDataSink
{
public:
    void put (const Streamline &data) { writeStreamline(data); }
};

class LabelledTrackvisDataSink : public TrackvisDataSink
{
protected:
    std::ofstream auxFileStream;
    BinaryOutputStream auxBinaryStream;
    std::map<int,std::string> labelDictionary;
    
public:
    LabelledTrackvisDataSink ()
    {
        auxBinaryStream.attach(&auxFileStream);
        auxBinaryStream.swapEndianness(false);
    }
    
    LabelledTrackvisDataSink (const std::string &fileStem, const NiftiImage &image, const std::map<int,std::string> labelDictionary)
        : labelDictionary(labelDictionary)
    {
        auxBinaryStream.attach(&auxFileStream);
        auxBinaryStream.swapEndianness(false);
        attach(fileStem, image);
    }
    
    ~LabelledTrackvisDataSink ()
    {
        auxBinaryStream.detach();
        if (auxFileStream.is_open())
            auxFileStream.close();
    }
    
    void attach (const std::string &fileStem, const NiftiImage &image);
    void put (const Streamline &data);
    void done ();
};

class MedianTrackvisDataSink : public TrackvisDataSink
{
protected:
    double quantile;
    Streamline median;
    
public:
    MedianTrackvisDataSink ()
        : TrackvisDataSink() {}
    
    MedianTrackvisDataSink (const std::string &fileStem, const NiftiImage &image, const double quantile = 0.99)
        : TrackvisDataSink(fileStem,image), quantile(quantile) {}
    
    void setup (const size_type &count, const_iterator begin, const_iterator end);
    void done ();
};

#endif
