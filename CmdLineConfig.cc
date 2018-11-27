/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *
 *****************************************************************************/

/*!
  \file   CmdLineConfig.cc
  \brief

  <long description>

  \author Volker Hejny
  \date   2002-07-29
*/

#include "CmdLineConfig.hh"
// #include "CConstBase.hh"
// #include "CLog.hh"

// #include "RSEnv.hh"
#include "TString.h"

#include <iostream>

ClassImp(CmdLineConfig);

static CmdLineOption o1("Name", "-n", "set sorter name", (const char*)0);
static CmdLineOption o2("Password", "-pw", "set sorter password",
                        (const char*)0);
static CmdLineOption o3("Logfile", "-lf", "set logfile name", (const char*)0);
static CmdLineOption o4("InputFile", "-fin", "set input source", "file:");
static CmdLineOption o4a("OutputFile", "-fout", "set output destination",
                         (const char*)0);
static CmdLineOption o5("RunNumber", "-r", "set run number to use", -1);
static CmdLineOption o6("ValidSince", "-vs", "set ValidSince manually",
                        (const char*)0);
static CmdLineOption o8("AutoAbort", "-abort", "abort analysis after EOF",
                        kFALSE);
static CmdLineOption o9("IncludePath", "-incpath", "include path for rc files",
                        (const char*)0);
static CmdLineOption o9a("DefaultPath", "-defpath", "default path for rc files",
                         "~/" /*DEFAULTPATH*/);
static CmdLineOption o0("Include", "", "", (const char*)0);

static CmdLineOption t1("ServerHost", "", "",
                        "localhost" /*CConstBase::ServerHost()*/);
static CmdLineOption t2("ServerPort", "", "",
                        1234 /*CConstBase::ServerPort()*/);

// TO BE DEFINED IN LOCAL PACKAGES !!!
//
// static CmdLineOption t3("SqlUrl"      ,"","","");
// static CmdLineOption t4("SqlUser"     ,"","","");
// static CmdLineOption t5("SqlPassword" ,"","","");

static CmdLineOption t6("ParameterDirectory", "", "", "./");

static CmdLineOption datadir("DataDir", "-dd", "Set path to data directory",
                             "./share");

CmdLineConfig* gCmdLineConfig = new CmdLineConfig();

CmdLineConfig::CmdLineConfig(){};

CmdLineConfig::~CmdLineConfig(){};

void CmdLineConfig::ReadCmdLine(int argc, char** argv) {
  CmdLineOption::ReadCmdLine(argc, argv);
}

ParameterSource CmdLineConfig::GetParameterSource() {
  TString mode = CmdLineOption::GetEnv()->GetValue(
      "CmdLine.ParameterSource", "sql" /*CConstBase::ParSource()*/);
  if (mode == "sql")
    return kSql;
  else if (mode == "file")
    return kFile;
  else if (mode == "fileimport")
    return kFileImport;
  else {
    std::cerr << "CmdLineConfig: unknown default parameter source '" << mode
              << "'\n"
              << "              falling back to SQL" << std::endl;
    return kSql;
  }
}

ParameterSource CmdLineConfig::GetParameterSourceType(const char* name) {
  TString query = "CmdLine.ParSource.";
  query += name;

  TString mode = CmdLineOption::GetEnv()->GetValue(
      query, CmdLineOption::GetEnv()->GetValue(
                 "CmdLine.ParameterSource", "sql" /*CConstBase::ParSource()*/));
  if (gDebug)
    std::cout << "CmdLineConfig: Query for parameter source type'" << query
              << "'\n"
              << "              returned '" << mode << "'" << std::endl;

  if (mode == "sql")
    return kSql;
  else if (mode == "file")
    return kFile;
  else if (mode == "fileimport")
    return kFileImport;
  else {
    return kImportExport;
  }
}

const TString CmdLineConfig::GetParameterSource(const char* name) {
  TString query = "CmdLine.ParSource.";
  query += name;
  const char* res =
      CmdLineOption::GetEnv()->GetValue(query, static_cast<const char*>(0));
  if (gDebug)
    std::cout << "CmdLineConfig: Query for parameter source '" << query << "'\n"
              << "              returned '" << res << "'" << std::endl;
  return res;
}

void CmdLineConfig::SetParameterSource(const char* name, const char* source) {
  TString query = "CmdLine.ParSource.";
  query += name;
  CmdLineOption::GetEnv()->SetValue(query, source);
}

ParameterSource CmdLineConfig::GetParameterDrain() {
  TString mode = CmdLineOption::GetEnv()->GetValue(
      "CmdLine.ParameterDrain", "file" /*CConstBase::ParDrain()*/);
  if (mode == "sql")
    return kSql;
  else if (mode == "file")
    return kFile;
  else {
    std::cerr << "CmdLineConfig: unknown default parameter source '" << mode
              << "'\n"
              << "              falling back to File" << std::endl;
    return kFile;
  }
}

ParameterSource CmdLineConfig::GetParameterDrainType(const char* name) {
  TString query = "CmdLine.ParDrain.";
  query += name;

  TString mode = CmdLineOption::GetEnv()->GetValue(
      query, CmdLineOption::GetEnv()->GetValue(
                 "CmdLine.ParameterDrain", "file" /*CConstBase::ParDrain()*/));
  if (gDebug)
    std::cout << "CmdLineConfig: Query for parameter drain type'" << query
              << "'\n"
              << "              returned '" << mode << "'" << std::endl;

  if (mode == "sql")
    return kSql;
  else if (mode == "file")
    return kFile;
  else {
    return kImportExport;
  }
};

const TString CmdLineConfig::GetParameterDrain(const char* name) {
  TString query = "CmdLine.ParDrain.";
  query += name;
  const char* res =
      CmdLineOption::GetEnv()->GetValue(query, static_cast<const char*>(0));
  if (gDebug)
    std::cout << "CmdLineConfig: Query for parameter source '" << query << "'\n"
              << "              returned '" << res << "'" << std::endl;
  return res;
};

void CmdLineConfig::SetParameterDrain(const char* name, const char* drain) {
  TString query = "CmdLine.ParDrain.";
  query += name;
  CmdLineOption::GetEnv()->SetValue(query, drain);
}

const TString CmdLineConfig::GetResource(const char* path, const char* file,
                                         EAccessMode mode) {
  char* resource = gSystem->Which(path, file, mode);
  if (!resource) { return ""; }
  return resource;
}
