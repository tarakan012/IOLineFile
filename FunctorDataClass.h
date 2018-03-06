
#ifndef TESTAURIGA_FUNCTORDATASTRUCT_H
#define TESTAURIGA_FUNCTORDATASTRUCT_H

#include <string>
#include <fstream>
#include <iostream>
#include <thread>

class COpenFileError : public std::runtime_error
{
public:
    explicit COpenFileError( const std::string & message)
    : std::runtime_error(message)
    {

    }
};

class CGetLineError : public std::runtime_error
{
public:
    explicit CGetLineError( const std::string & message)
            : std::runtime_error(message)
    {

    }
};

class CFunctorBase
{
public:
    void Open(std::string strNameFile)
    {
        m_File.open(strNameFile);
        if(!m_File.is_open())
        {
            throw COpenFileError("Error opening file");
        }
    }
    void SetStrLine(const std::string & str)
    {
        m_strLine = str;
    }
    std::string GetStrLine() const
    {
        return m_strLine;
    }
    virtual std::string operator()() = 0;
protected:
    std::fstream m_File;
    std::string m_strLine;
};

class CFunctorReadLineFromFile : public CFunctorBase
{
public:
    std::string operator()()
    {
        if(!std::getline(m_File, m_strLine))
        {
            throw CGetLineError("Get line error");
        }
        std::cout << "string : " << m_strLine << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
        return m_strLine;
    }
};

class CFunctorWriteLineFromFile : public CFunctorBase
{
public:
    void Open(std::string strNameFile)
    {
        m_File.open(strNameFile);
        if(!m_File.is_open())
        {
            throw COpenFileError("Error opening file");
        }
    }
    std::string operator()()
    {
        m_File << m_strLine<<"\n";
        return std::string();
    }
};

#endif //TESTAURIGA_FUNCTORDATASTRUCT_H
