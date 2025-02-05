#ifndef _PCOLORER_H_
#define _PCOLORER_H_

/**
  Visual Leak Detector http://sites.google.com/site/dmoulding/vld .
  Optional 
*/
#ifdef _DEBUG
#ifndef _WIN64
//#include<vld.h>
#endif
#endif

#include<farplug-wide.h>
#include<farcolor.h>
#include<farkeys.h>
#include"ConsoleAnnotation.h"

#include<wctype.h>
#include<wchar.h>
#include<windows.h>
#if 0
#include <g3log/logworker.hpp>
#endif
// new colorer don't have class StringBuffer
#include "StringBuffer.h"

extern PluginStartupInfo Info;
extern FarStandardFunctions FSF;
extern StringBuffer *PluginPath;
#if 0
extern std::unique_ptr<g3::LogWorker> logworker;
#endif

/** FAR .lng file identifiers. */
enum
{
  mName, mSetup, mTurnOff, mTrueMod,
  mCross, mPairs, mSyntax, mOldOutline,
  mOk, mReloadAll, mCancel,
  mCatalogFile, mHRDName, mHRDNameTrueMod,
  mListTypes, mMatchPair, mSelectBlock, mSelectPair,
  mListFunctions, mFindErrors, mSelectRegion, mRegionInfo,
  mLocateFunction, mUpdateHighlight, mReloadBase, mConfigure,
  mTotalTypes, mSelectSyntax, mOutliner, mNothingFound,
  mGotcha, mChoose,
  mReloading, mCantLoad, mCantOpenFile, mDie, mTry,
  mFatal, mSelectHRD, mChangeBackgroundEditor, mTrueModSetting,
  mNoFarTM, mNoConEmu, mNoFarTMConEmu, mUserHrdFile, mUserHrcFile, mUserHrcSetting,
  mUserHrcSettingDialog, mListSyntax, mParamList,mParamValue, mAutoDetect, mFavorites,
  mKeyAssignDialogTitle, mKeyAssignTextTitle
};

StringBuffer *GetConfigPath(const SString &sub);

#endif
/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is the Colorer Library.
 *
 * The Initial Developer of the Original Code is
 * Cail Lomecb <irusskih at gmail.com>.
 * Portions created by the Initial Developer are Copyright (C) 1999-2005
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */
