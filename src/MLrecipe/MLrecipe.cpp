#include "MLrecipe.h"
#include "DispatchRecipeModule.h"
MLrecipe::MLrecipe(QObject* parent) : QObject(parent)
{
	DispatchRecipeModule* dispatch = new DispatchRecipeModule();

	m_recipeWidget = new RecipeEngineWidget(dispatch);
}

QWidget* MLrecipe::GetWidget()
{
	return m_recipeWidget;
}