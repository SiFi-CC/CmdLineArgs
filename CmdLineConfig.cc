/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *    Rafa? Lalik                 Modifications, creation of CmdLineArgs library
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *    Copyright (C) 2018          Rafa? Lalik
 *
 *****************************************************************************/

/*!
  \file   CmdLineConfig.cc
  \brief

  <long description>

  \author Volker Hejny
  \date   2002-07-29
  \author Rafa? Lalik
  \date   2018-11-27
*/

#include <TEnv.h>
#include <TString.h>

#include <iostream>

#include "CmdLineConfig.hh"

ClassImp(CmdLineConfig);

static CmdLineOption t6("ParameterDirectory", "", "", "./");

static CmdLineOption datadir("DataDir", "-dd", "Set path to data directory",
                             "./share");

CmdLineConfig::CmdLineConfig(){};

CmdLineConfig::~CmdLineConfig(){};

CmdLineConfig * CmdLineConfig::inst = nullptr;

CmdLineConfig * CmdLineConfig::instance()
{
  if (!CmdLineConfig::inst)
    inst = new CmdLineConfig();

  return inst;
}


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
