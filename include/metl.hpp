// File: metl.h
// 
// Copyright 2017-2018 Till Heinzel
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "src/API/MakeCompiler.hpp"
#include "src/API/addDefaults.hpp"

#include "src/Stack.impl.hpp"
#include "src/SubStack.impl.hpp"
#include "src/FunctionSubStack.impl.hpp"
#include "src/ExpressionSubStack.impl.hpp"
#include "src/DataBase/CompilerEntityDataBase.impl.hpp"
#include "src/Caster.impl.hpp"
#include "src/OperationApplier.impl.hpp"

#include "src/TypeErasure/UntypedOperation.impl.hpp"
#include "src/TypeErasure/UntypedConstant.impl.hpp"
#include "src/TypeErasure/UntypedVariable.impl.hpp"
#include "src/TypeErasure/UntypedValue.impl.hpp"
#include "src/TypeErasure/UntypedExpression.impl.hpp"
