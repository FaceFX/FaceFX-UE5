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

static FName s_BrushIdActor(TEXT("FaceFXStyle.AssetFXActor"));
static FName s_BrushIdAnim(TEXT("FaceFXStyle.AssetFXAnim"));
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

#define FACEFX_ICON_PATH( RelativePath ) (ResourceDir / RelativePath)

	const FVector2D Icon40(40.F, 40.F);
    StyleSet->Set(s_BrushIdActor, new FSlateImageBrush(FACEFX_ICON_PATH(TEXT("Icons/facefxactor.png")), Icon40));
    StyleSet->Set(s_BrushIdAnim, new FSlateImageBrush(FACEFX_ICON_PATH(TEXT("Icons/facefxanim.png")), Icon40));

	const FVector2D Icon16(16.F, 16.F);
    StyleSet->Set(s_BrushIdSuccess, new FSlateImageBrush(FACEFX_ICON_PATH(TEXT("Icons/facefxsuccess.png")), Icon16));
    StyleSet->Set(s_BrushIdWarn, new FSlateImageBrush(FACEFX_ICON_PATH(TEXT("Icons/facefxwarning.png")), Icon16));
    StyleSet->Set(s_BrushIdError, new FSlateImageBrush(FACEFX_ICON_PATH(TEXT("Icons/facefxerror.png")), Icon16));

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

const FName& FFaceFXStyle::GetBrushIdFxActor()
{
	return s_BrushIdActor;
}

const FName& FFaceFXStyle::GetBrushIdFxAnim()
{
	return s_BrushIdAnim;
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
