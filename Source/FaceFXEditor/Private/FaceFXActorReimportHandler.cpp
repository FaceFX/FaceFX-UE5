/*******************************************************************************
  The MIT License (MIT)
  Copyright (c) 2015-2026 Speech Graphics Ltd. All rights reserved.
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*******************************************************************************/

#include "FaceFXActorReimportHandler.h"

#include "FaceFX.h"
#include "FaceFXAsset.h"
#include "FaceFXActor.h"
#include "FaceFXEditorConfig.h"
#include "FaceFXEditorTools.h"
#include "Factories/FaceFXActorFactory.h"
#include "Include/Slate/FaceFXResultWidget.h"
#include "Misc/FeedbackContext.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FaceFX"

// Called if the dropped .facefx file is associated with an existing FaceFX Actor.
bool FFaceFXActorReimportHandler::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
    // We only support re-importing FaceFX Actor (.facefx) assets this way.
    const UFaceFXActor* FaceFXActor = Cast<UFaceFXActor>(Obj);
    if (!FaceFXActor)
    {
        return false;
    }

    const FString Filename = FaceFXActor->GetAssetPath();
    if (Filename.IsEmpty())
    {
        return false;
    }

    // FaceFX Actor assets have one .facefx source association.
    OutFilenames.Add(Filename);

    return true;
}

// We don't support changing the source path as we only support re-import via drag-and-drop. This is required
// by the interface, but is a no-op here.
void FFaceFXActorReimportHandler::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
    // no-op
    const UFaceFXActor* FaceFXActor = Cast<UFaceFXActor>(Obj);
    if (!FaceFXActor)
    {
        UE_LOG(LogFaceFX, Verbose, TEXT("FaceFX reimport path change ignored for non-FaceFXActor: %s"), *GetNameSafe(Obj));
        return;
    }

    if (NewReimportPaths.Num() != 1)
    {
        UE_LOG(LogFaceFX, Verbose, TEXT("FaceFX reimport path change ignored for %s; expected one source path, got %d."),
               *FaceFXActor->GetPathName(), NewReimportPaths.Num());
        return;
    }

    const FString& CurrentPath = FaceFXActor->GetAssetPath();
    const FString& RequestedPath = NewReimportPaths[0];

    if (RequestedPath.IsEmpty())
    {
        UE_LOG(LogFaceFX, Verbose, TEXT("FaceFX reimport path change ignored for %s; requested path is empty."), *FaceFXActor->GetPathName());
        return;
    }

    if (!FPaths::IsSamePath(CurrentPath, RequestedPath))
    {
        UE_LOG(LogFaceFX, Warning, TEXT("FaceFX does not support changing an imported actor's source file through reimport. Ignoring requested path for %s: %s (continuing to use: %s)"),
               *FaceFXActor->GetPathName(), *RequestedPath, *CurrentPath);
    }
}

namespace
{

    void HandleFaceFXActorReimportBeforeDelete(UObject* Asset, const FString& CompilationFolder, bool bLoadResult, FFaceFXImportResult& OutResultMessages)
    {
        if (bLoadResult)
        {
            UFaceFXActorFactory::HandleFaceFXActorCreated(CastChecked<UFaceFXActor>(Asset), CompilationFolder, OutResultMessages);
        }
    }

}

// Performs the re-import in response to drag-and-drop of the same .facefx file on the Content Browser (if CanReimport
// returns true).
EReimportResult::Type FFaceFXActorReimportHandler::Reimport(UObject* Obj)
{
    // We only support re-importing FaceFX Actor (.facefx) assets this way.
    UFaceFXActor* FaceFXActor = Cast<UFaceFXActor>(Obj);
    if (!FaceFXActor)
    {
        return EReimportResult::Failed;
    }

    const FString Filename = FaceFXActor->GetAssetPath();
    if (Filename.IsEmpty() || !FPaths::FileExists(Filename))
    {
        UE_LOG(LogFaceFX, Warning, TEXT("Cannot reimport FaceFX actor %s: source file is missing: %s"), *FaceFXActor->GetPathName(), *Filename);
        return EReimportResult::Failed;
    }

    FFaceFXImportResultSet ResultSet;
    FFaceFXImportResult& Result = ResultSet.GetOrAdd(FaceFXActor);

    FCompilationBeforeDeletionDelegate DeletionDelegate;
    if (UFaceFXEditorConfig::Get().IsImportAnimationOnActorImport())
    {
        // Actor assets may lead to changed animation sets
        DeletionDelegate = FCompilationBeforeDeletionDelegate::CreateStatic(&HandleFaceFXActorReimportBeforeDelete);
    }

	GWarn->BeginSlowTask(LOCTEXT("ReimportProgress", "Reimporting FaceFX Assets..."), true);

    const bool bSucceeded = FFaceFXEditorTools::ReImportFaceFXAsset(FaceFXActor, Result, DeletionDelegate);

	GWarn->UpdateProgress(1, 1);
	GWarn->EndSlowTask();

    FFaceFXResultWidget::Create(LOCTEXT("ShowNativeReimportResultTitle", "Reimport Result"), ResultSet);

    return bSucceeded ? EReimportResult::Succeeded : EReimportResult::Failed;
}

#undef LOCTEXT_NAMESPACE
