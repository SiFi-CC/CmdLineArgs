/******************************************************************************
 *
 * $Id$
 *
 * Environment:
 *    Software development for ANKE detector system at COSY
 *
 * Author List:
 *    Volker Hejny                Original author
 *    Rafał Lalik                 Modifications, creation of CmdLineArgs library
 *
 * Copyright Information:
 *    Copyright (C) 2002          Institut für Kernphysik
 *                                Forschungszentrum Jülich
 *    Copyright (C) 2018          Rafał Lalik, Jagiellonian University Kraków
 *
 *****************************************************************************/

/*!
  \file   CmdLineConfig.cc
  \brief

  <long description>

  \author Volker Hejny
  \date   2002-07-29
  \author Rafał Lalik
  \date   2018-11-27
*/

#include <TEnv.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>

#include <cstdlib>
#include <iostream>

#include "CmdLineConfig.hh"

TEnv* CmdLineConfig::fgEnv = nullptr;
CmdLineConfig::Options CmdLineConfig::fgOpts;
Positional CmdLineConfig::fgArgs;
Greedy CmdLineConfig::fgGreedy;
TString CmdLineConfig::fPosText = "[...]";
Int_t CmdLineConfig::fGreedyPosition = -1;
CmdLineArg* CmdLineConfig::fGreedy = nullptr;

CmdLineConfig::ListMap CmdLineConfig::_map_opts;
CmdLineConfig::ListMap CmdLineConfig::_map_args;

static CmdLineOption t6("ParameterDirectory", "", "", "./");

static CmdLineOption datadir("DataDir", "-dd", "Set path to data directory",
                             "./share");

CmdLineConfig::CmdLineConfig() : name(".cmdlinerc"){};

CmdLineConfig::CmdLineConfig(const char* name) : name(name){};

CmdLineConfig::~CmdLineConfig(){};

CmdLineConfig* CmdLineConfig::inst = nullptr;

CmdLineConfig* CmdLineConfig::instance(const char* name) {
  if (!CmdLineConfig::inst) {
    if (name)
      inst = new CmdLineConfig(name);
    else
      inst = new CmdLineConfig();
  }

  return inst;
}

void CmdLineConfig::ReadCmdLine(int argc, char** argv) {
  GetEnv();

  fgGreedy.erase(fgGreedy.begin(), fgGreedy.end());
  fgGreedy.clear();

  std::vector<TString> positional;

  for (Int_t i = 1; i < argc; i++) {
    Bool_t isCmdLine = kFALSE;
    if (CheckCmdLineSpecial(argc, argv, i)) continue;

    Options::const_iterator it = fgOpts.begin();

    while (it != fgOpts.end()) {
      CmdLineOption* entry = (it++)->second;
      if (entry->fCmdArg == "") continue;

      if (entry->fCmdArg == argv[i]) {
        isCmdLine = kTRUE;
        if (entry->fType == CmdLineOption::kFlag)
          CmdLineConfig::instance()->GetEnv()->SetValue(
              "CmdLine." + entry->fName, kTRUE);
        else if (i < argc - 1)
          CmdLineConfig::instance()->GetEnv()->SetValue(
              "CmdLine." + entry->fName, argv[++i]);
        if (entry->fFunction != 0) (*entry->fFunction)();
        break;
      }
    }

    if (!isCmdLine) positional.push_back(argv[i]);
  }

  int greedy_len = positional.size() - fgArgs.size();
  if (greedy_len < 0) {
    std::cerr << "Not enough positional arguments. Needed " << fgArgs.size()
              << ", given " << positional.size() << std::endl;
    abort();
  }

  Positional::iterator pit = fgArgs.begin();
  Int_t greedy_end = fGreedyPosition + greedy_len - 1;

  ListMap::iterator ait = _map_args.begin();

  for (int i = 0; i < positional.size(); ++i) {
    if (i < fGreedyPosition) {
      fgArgs[*ait++]->fValue = positional[i];
    } else if (i > greedy_end) {
      fgArgs[*ait++]->fValue = positional[i];
    } else {
      CmdLineArg* greedy =
          new CmdLineArg("", "", fGreedy->fType, nullptr, true);
      greedy->fValue = positional[i];
      fgGreedy.push_back(greedy);
    }
  }
}

ParameterSource CmdLineConfig::GetParameterSource() {
  if (nullptr == inst) instance();

  TString mode = inst->GetEnv()->GetValue("CmdLine.ParameterSource",
                                          "sql" /*CConstBase::ParSource()*/);
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
  if (nullptr == inst) instance();

  TString query = "CmdLine.ParSource.";
  query += name;

  TString mode = inst->GetEnv()->GetValue(
      query, inst->GetEnv()->GetValue("CmdLine.ParameterSource",
                                      "sql" /*CConstBase::ParSource()*/));
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
  if (nullptr == inst) instance();

  TString query = "CmdLine.ParSource.";
  query += name;
  const char* res =
      inst->GetEnv()->GetValue(query, static_cast<const char*>(0));
  if (gDebug)
    std::cout << "CmdLineConfig: Query for parameter source '" << query << "'\n"
              << "              returned '" << res << "'" << std::endl;
  return res;
}

void CmdLineConfig::SetParameterSource(const char* name, const char* source) {
  if (nullptr == inst) instance();

  TString query = "CmdLine.ParSource.";
  query += name;
  inst->GetEnv()->SetValue(query, source);
}

ParameterSource CmdLineConfig::GetParameterDrain() {
  if (nullptr == inst) instance();

  TString mode = inst->GetEnv()->GetValue("CmdLine.ParameterDrain",
                                          "file" /*CConstBase::ParDrain()*/);
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
  if (nullptr == inst) instance();

  TString query = "CmdLine.ParDrain.";
  query += name;

  TString mode = inst->GetEnv()->GetValue(
      query, inst->GetEnv()->GetValue("CmdLine.ParameterDrain",
                                      "file" /*CConstBase::ParDrain()*/));
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
  if (nullptr == inst) instance();

  TString query = "CmdLine.ParDrain.";
  query += name;
  const char* res =
      inst->GetEnv()->GetValue(query, static_cast<const char*>(0));
  if (gDebug)
    std::cout << "CmdLineConfig: Query for parameter source '" << query << "'\n"
              << "              returned '" << res << "'" << std::endl;
  return res;
};

void CmdLineConfig::SetParameterDrain(const char* name, const char* drain) {
  if (nullptr == inst) instance();

  TString query = "CmdLine.ParDrain.";
  query += name;
  inst->GetEnv()->SetValue(query, drain);
}

const TString CmdLineConfig::GetResource(const char* path, const char* file,
                                         EAccessMode mode) {
  char* resource = gSystem->Which(path, file, mode);
  if (!resource) { return ""; }
  return resource;
}

TEnv* CmdLineConfig::GetEnv() {
  if (fgEnv != 0) return fgEnv;

  fgEnv = new TEnv(name.Data());
  TString defaultpath = "";
  if (CmdLineOption::GetStringValue("DefaultPath")) {
    defaultpath =
        gSystem->ExpandPathName(CmdLineOption::GetStringValue("DefaultPath"));
    if (gSystem->AccessPathName(defaultpath)) {
      std::cerr << "Error: default path not accessible (" << defaultpath << ")"
                << std::endl;
    } else {
      void* dirp = gSystem->OpenDirectory(defaultpath);
      if (dirp == 0) {
        std::cerr << "Error: default path not readable (" << defaultpath << ")"
                  << std::endl;
      } else {
        std::cout << "Reading defaults from " << defaultpath << std::endl;
        if (!defaultpath.EndsWith("/")) defaultpath += "/";
        const char* localname = 0;
        while ((localname = gSystem->GetDirEntry(dirp))) {
          TString strName = localname;
          if (strName.EndsWith(".rc")) {
            fgEnv->ReadFile(defaultpath + strName, kEnvGlobal);
          }
        }
        gSystem->FreeDirectory(dirp);
      }
    }
  }
  TString includepath = "";
  if (CmdLineOption::GetStringValue("IncludePath")) {
    includepath =
        gSystem->ExpandPathName(CmdLineOption::GetStringValue("IncludePath"));
    if (!(includepath.EndsWith("/"))) includepath += "/";
  }
  if (CmdLineOption::GetStringValue("Include")) {
    TString includes =
        gSystem->ExpandPathName(CmdLineOption::GetStringValue("Include"));
    TObjArray* includesArray = includes.Tokenize(" ");
    TObjString* objString;
    TIter it(includesArray);
    while ((objString = dynamic_cast<TObjString*>(it.Next()))) {
      TString filename = objString->GetString();
      if (!(filename.BeginsWith("/") || filename.BeginsWith("./"))) {
        filename = includepath + filename;
      }
      if (gSystem->AccessPathName(filename)) {
        std::cerr << "Error: rc-file not readable (" << filename << ")"
                  << std::endl;
      } else {
        void* dirp = gSystem->OpenDirectory(filename);
        if (dirp == 0) {
          std::cout << "Reading " << filename << std::endl;
          fgEnv->ReadFile(filename, kEnvUser);
        } else {
          if (!filename.EndsWith("/")) filename += "/";
          const char* localname = 0;
          while ((localname = gSystem->GetDirEntry(dirp))) {
            TString strName = localname;
            if (strName.EndsWith(".rc")) {
              std::cout << "Reading " << filename + strName << std::endl;
              fgEnv->ReadFile(filename + strName, kEnvUser);
            }
          }
          gSystem->FreeDirectory(dirp);
        }
      }
    }
    delete includesArray;
  }
  // work-around because values in these files are overwritten by
  // values in "Defaults" directory
  char* s = gSystem->ConcatFileName(gSystem->HomeDirectory(), name.Data());
  fgEnv->ReadFile(s, kEnvChange);
  delete[] s;
  fgEnv->ReadFile(name.Data(), kEnvChange);
  return fgEnv;
}

void CmdLineConfig::ClearOptions() {
  //   for (int i = fgOpts.size(); i > 2; --i) { FIXME
  //     CmdLineOption* obj = fgOpts.back();
  //     fgOpts.pop_back();
  //     delete obj;
  //     obj = nullptr;
  //   }

  //   Options::iterator it = fgOpts.begin();
  //   while (it != fgOpts.end()) {
  //     delete it->second;
  //     ++it;
  //   }

  fgOpts.clear();
  fgArgs.clear();
  fgGreedy.clear();
  fGreedyPosition = -1;
  _map_args.clear();
  _map_opts.clear();
}

CmdLineOption* CmdLineConfig::FindOption(const char* name) {
  if (0 == fgOpts.size()) return nullptr;

  Options::const_iterator it = fgOpts.find(name);
  if (fgOpts.end() != it) return it->second;

  return nullptr;
}

CmdLineArg* CmdLineConfig::FindArgument(const char* name) {
  if (0 == fgArgs.size()) return nullptr;

  Positional::const_iterator it = fgArgs.find(name);
  if (fgArgs.end() != it) return it->second;

  return nullptr;
}

void CmdLineConfig::Insert(CmdLineOption* opt) {
  Options::const_iterator it = fgOpts.begin();

  while (it != fgOpts.end()) {
    CmdLineOption* entry = (it++)->second;

    if (entry->fName == opt->fName) {
      std::cerr << "CmdLineOption: option '" << opt->fName
                << "' already exists -> fix it" << std::endl;
      exit(1);
    }
    if (opt->fCmdArg != "" && entry->fCmdArg == opt->fCmdArg) {
      std::cerr << "CmdLineOption: options '" << opt->fName << "' and '"
                << entry->fName << "' share tag '" << opt->fCmdArg << "'"
                << std::endl;
      exit(1);
    }
  }

  fgOpts[opt->fName] = opt;
  _map_opts.push_back(opt->fName.Data());
}

void CmdLineConfig::Insert(CmdLineArg* arg) {
  if (0 == arg->fName.Length()) {
    if (fGreedyPosition >= 0) {
      std::cerr << "Only one greedy parameter allowed." << std::endl;
      abort();
    }
    fGreedyPosition = fgArgs.size();
    fGreedy = arg;
    return;
  }

  Positional::const_iterator it = fgArgs.begin();

  while (it != fgArgs.end()) {
    CmdLineArg* entry = (it++)->second;
    if (entry->fName == arg->fName) {
      std::cerr << "CmdLineOption: argument '" << arg->fName
                << "' already exists -> fix it" << std::endl;
      exit(1);
    }
  }

  fgArgs.insert(std::pair<std::string, CmdLineArg*>(arg->fName.Data(), arg));
  _map_args.push_back(arg->fName.Data());
}

Bool_t CmdLineConfig::CheckCmdLineSpecial(int argc, char** argv, int i) {
  TString option = argv[i];
  if (option == "-h") {
    PrintHelp(argc, argv);
    exit(EXIT_SUCCESS);
    return kTRUE;
  } else if (option == "-p") {
    Print();
    return kTRUE;
  } else if (option == "-extra-sorterrc") {
    if ((i + 1) < argc) {
      TString includepath = "";
      if (CmdLineOption::GetStringValue("IncludePath")) {
        includepath = gSystem->ExpandPathName(
            CmdLineOption::GetStringValue("IncludePath"));
        if (!(includepath.EndsWith("/"))) includepath += "/";
      }
      TString extrafile = argv[i + 1];
      TString extra = gSystem->ExpandPathName(extrafile.Data());
      if (!(extra.BeginsWith("/") || extra.BeginsWith("./"))) {
        extra = includepath + extra;
      }
      if (gSystem->AccessPathName(extra)) {
        std::cout << "Error: extra rc path not accessible (" << extra << ")"
                  << std::endl;
        exit(EXIT_SUCCESS);
      } else {
        void* dirp = gSystem->OpenDirectory(extra);
        if (dirp == 0) {
          std::cout << "Reading extra rc file: " << extra << std::endl;
          CmdLineConfig::instance()->GetEnv()->ReadFile(extra, kEnvChange);
        } else {
          if (!extra.EndsWith("/")) extra += "/";
          const char* localname = 0;
          while ((localname = gSystem->GetDirEntry(dirp))) {
            TString strName = localname;
            if (strName.EndsWith(".rc")) {
              std::cout << "Reading extra fc file: " << extra + strName
                        << std::endl;
              CmdLineConfig::instance()->GetEnv()->ReadFile(extra + strName,
                                                            kEnvChange);
            }
          }
          gSystem->FreeDirectory(dirp);
        }
      }
    }
    return kTRUE;
  } // end extra sorterrc
  return kFALSE;
}

void CmdLineConfig::PrintHelp(int argc, char** argv) {
  std::cout << "Usage: ";
  if (argc and argv)
    std::cout << argv[0];
  else
    std::cout << "this_app";
  std::cout << " [options]";

  if (fgArgs.size()) {
    Int_t pos = 0;
    ListMap::const_iterator ait = _map_args.begin();

    while (ait != _map_args.end()) {
      if (fGreedyPosition == pos) std::cout << " " << fPosText;
      std::cout << " " << *ait++;
      ++pos;
    }
    if (pos == fGreedyPosition) std::cout << " " << fPosText;
  }
  std::cout << std::endl;
  std::cout << "  -h                  show this help" << std::endl;

  if (0 == fgOpts.size()) return;

  ListMap::const_iterator oit = _map_opts.begin();
  while (oit != _map_opts.end())
    fgOpts[*oit++]->PrintHelp();

  ListMap::const_iterator ait = _map_args.begin();

  Int_t pos = 0;
  while (ait != _map_args.end()) {
    if (fGreedyPosition == pos)
      if (fGreedy) fGreedy->PrintHelp(fPosText);
    fgArgs[*ait++]->PrintHelp();
    ++pos;
  }

  if (pos == fGreedyPosition)
    if (fGreedy) fGreedy->PrintHelp(fPosText);
}

void CmdLineConfig::Print() {
  if (0 == fgOpts.size()) return;
  std::cout << "Current settings:" << std::endl;

  Options::const_iterator it = fgOpts.begin();
  CmdLineOption* entry;
  while ((entry = (*it++).second)) {
    entry->Print();
  }
}

void CmdLineConfig::RestoreDefaults() {
  instance()->GetEnv();

  Options::const_iterator it = fgOpts.begin();

  while (it != fgOpts.end()) {
    CmdLineOption* entry = (it++)->second;
    if (entry->fCmdArg == "") continue;

    switch (entry->fType) {
      case CmdLineOption::kFlag:
        CmdLineConfig::instance()->GetEnv()->SetValue("CmdLine." + entry->fName,
                                                      kFALSE);
        break;
      case CmdLineOption::kBool:
      case CmdLineOption::kInt:
        CmdLineConfig::instance()->GetEnv()->SetValue("CmdLine." + entry->fName,
                                                      entry->fDefInt);
        break;
      case CmdLineOption::kDouble:
        CmdLineConfig::instance()->GetEnv()->SetValue("CmdLine." + entry->fName,
                                                      entry->fDefDouble);
        break;
      case CmdLineOption::kString:
      case CmdLineOption::kStringNotChecked:
        CmdLineConfig::instance()->GetEnv()->SetValue("CmdLine." + entry->fName,
                                                      entry->fDefString);
        break;
      default:
        break;
    }
  }
}
