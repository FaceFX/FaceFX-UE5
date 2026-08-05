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

#include "FaceFXStyle.h"

#include "ClassIconFinder.h"
#include "FaceFXEditor.h"
#include "FaceFXEditorConfig.h"
#include "Interfaces/IPluginManager.h"
#include "Styling/SlateBrush.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleRegistry.h"

static FName s_BrushIdActorIcon(TEXT("ClassIcon.FaceFXActor"));
static FName s_BrushIdActorThumbnail(TEXT("ClassThumbnail.FaceFXActor"));
static FName s_BrushIdAnimIcon(TEXT("ClassIcon.FaceFXAnim"));
static FName s_BrushIdAnimThumbnail(TEXT("ClassThumbnail.FaceFXAnim"));
static FName s_BrushIdSuccess(TEXT("FaceFXStyle.IconSuccess"));
static FName s_BrushIdWarn(TEXT("FaceFXStyle.IconWarn"));
static FName s_BrushIdError(TEXT("FaceFXStyle.IconError"));

TSharedPtr<FSlateStyleSet> FFaceFXStyle::StyleSet;

/** Initializes the style set */
void FFaceFXStyle::Initialize()
{
	// Only register once
	if (StyleSet.IsValid())
	{
		return;
	}

	StyleSet = MakeShareable(new FSlateStyleSet("FaceFXStyle"));
	StyleSet->SetContentRoot(FPaths::EngineContentDir() / TEXT("Editor/Slate"));
	StyleSet->SetCoreContentRoot(FPaths::EngineContentDir() / TEXT("Slate"));

	const TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin(TEXT("FaceFX"));
	check(Plugin.IsValid());

	const FString ResourceDir = Plugin->GetBaseDir() / TEXT("Resources");

#define FACEFX_ICON( RelativePath, Size ) new FSlateImageBrush(ResourceDir / TEXT(RelativePath), Size)

	const FVector2D Icon16(16.F, 16.F);
    StyleSet->Set(s_BrushIdSuccess, FACEFX_ICON("Icons/facefxsuccess.png", Icon16));
    StyleSet->Set(s_BrushIdWarn, FACEFX_ICON("Icons/facefxwarning.png", Icon16));
    StyleSet->Set(s_BrushIdError, FACEFX_ICON("Icons/facefxerror.png", Icon16));

    const FVector2D Icon64(64.F, 64.F);
    StyleSet->Set(s_BrushIdActorIcon, FACEFX_ICON("Icons/icon64.png", Icon16));
    StyleSet->Set(s_BrushIdActorThumbnail, FACEFX_ICON("Icons/icon16.png", Icon64));

    StyleSet->Set(s_BrushIdAnimIcon, FACEFX_ICON("Icons/icont64.png", Icon16));
    StyleSet->Set(s_BrushIdAnimThumbnail, FACEFX_ICON("Icons/icon16.png", Icon64));

#undef FACEFX_ICON_PATH

	FSlateStyleRegistry::RegisterSlateStyle(*StyleSet.Get());
};

/** Shutdown the style set */
void FFaceFXStyle::Shutdown()
{
	if (StyleSet.IsValid())
	{
		FSlateStyleRegistry::UnRegisterSlateStyle(*StyleSet.Get());
		ensure(StyleSet.IsUnique());
		StyleSet.Reset();
	}
}

const FName& FFaceFXStyle::GetBrushIdFxActorIcon()
{
	return s_BrushIdActorIcon;
}

const FName& FFaceFXStyle::GetBrushIdFxActorThumbnail()
{
	return s_BrushIdActorThumbnail;
}

const FName& FFaceFXStyle::GetBrushIdFxAnimIcon()
{
	return s_BrushIdAnimIcon;
}

const FName& FFaceFXStyle::GetBrushIdFxAnimThumbnail()
{
	return s_BrushIdAnimThumbnail;
}

const FSlateBrush* FFaceFXStyle::GetBrushStateIconSuccess()
{
	return StyleSet.IsValid() ? StyleSet->GetBrush(s_BrushIdSuccess) : nullptr;
}

const FSlateBrush* FFaceFXStyle::GetBrushStateIconWarning()
{
	return StyleSet.IsValid() ? StyleSet->GetBrush(s_BrushIdWarn) : nullptr;
}

const FSlateBrush* FFaceFXStyle::GetBrushStateIconError()
{
	return StyleSet.IsValid() ? StyleSet->GetBrush(s_BrushIdError) : nullptr;
}
