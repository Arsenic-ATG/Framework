// Copyright (c) 2021 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=fb09a7e7db1267d0653fff64c419ab573496614f$
//

#include "libcef_dll/ctocpp/download_item_callback_ctocpp.h"
#include "libcef_dll/shutdown_checker.h"

// VIRTUAL METHODS - Body may be edited by hand.

NO_SANITIZE("cfi-icall") void CefDownloadItemCallbackCToCpp::Cancel() {
  shutdown_checker::AssertNotShutdown();

  cef_download_item_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, cancel))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->cancel(_struct);
}

NO_SANITIZE("cfi-icall") void CefDownloadItemCallbackCToCpp::Pause() {
  shutdown_checker::AssertNotShutdown();

  cef_download_item_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, pause))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->pause(_struct);
}

NO_SANITIZE("cfi-icall") void CefDownloadItemCallbackCToCpp::Resume() {
  shutdown_checker::AssertNotShutdown();

  cef_download_item_callback_t* _struct = GetStruct();
  if (CEF_MEMBER_MISSING(_struct, resume))
    return;

  // AUTO-GENERATED CONTENT - DELETE THIS COMMENT BEFORE MODIFYING

  // Execute
  _struct->resume(_struct);
}

// CONSTRUCTOR - Do not edit by hand.

CefDownloadItemCallbackCToCpp::CefDownloadItemCallbackCToCpp() {}

// DESTRUCTOR - Do not edit by hand.

CefDownloadItemCallbackCToCpp::~CefDownloadItemCallbackCToCpp() {
  shutdown_checker::AssertNotShutdown();
}

template <>
cef_download_item_callback_t* CefCToCppRefCounted<
    CefDownloadItemCallbackCToCpp,
    CefDownloadItemCallback,
    cef_download_item_callback_t>::UnwrapDerived(CefWrapperType type,
                                                 CefDownloadItemCallback* c) {
  NOTREACHED() << "Unexpected class type: " << type;
  return nullptr;
}

template <>
CefWrapperType CefCToCppRefCounted<CefDownloadItemCallbackCToCpp,
                                   CefDownloadItemCallback,
                                   cef_download_item_callback_t>::kWrapperType =
    WT_DOWNLOAD_ITEM_CALLBACK;
