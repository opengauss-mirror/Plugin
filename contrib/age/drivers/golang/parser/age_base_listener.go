/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
// Code generated from Age.g4 by ANTLR 4.9.2. DO NOT EDIT.

package parser // Age

import "github.com/antlr/antlr4/runtime/Go/antlr"

// BaseAgeListener is a complete listener for a parse tree produced by AgeParser.
type BaseAgeListener struct{}

var _ AgeListener = &BaseAgeListener{}

// VisitTerminal is called when a terminal node is visited.
func (s *BaseAgeListener) VisitTerminal(node antlr.TerminalNode) {}

// VisitErrorNode is called when an error node is visited.
func (s *BaseAgeListener) VisitErrorNode(node antlr.ErrorNode) {}

// EnterEveryRule is called when any rule is entered.
func (s *BaseAgeListener) EnterEveryRule(ctx antlr.ParserRuleContext) {}

// ExitEveryRule is called when any rule is exited.
func (s *BaseAgeListener) ExitEveryRule(ctx antlr.ParserRuleContext) {}

// EnterAgeout is called when production ageout is entered.
func (s *BaseAgeListener) EnterAgeout(ctx *AgeoutContext) {}

// ExitAgeout is called when production ageout is exited.
func (s *BaseAgeListener) ExitAgeout(ctx *AgeoutContext) {}

// EnterVertex is called when production vertex is entered.
func (s *BaseAgeListener) EnterVertex(ctx *VertexContext) {}

// ExitVertex is called when production vertex is exited.
func (s *BaseAgeListener) ExitVertex(ctx *VertexContext) {}

// EnterEdge is called when production edge is entered.
func (s *BaseAgeListener) EnterEdge(ctx *EdgeContext) {}

// ExitEdge is called when production edge is exited.
func (s *BaseAgeListener) ExitEdge(ctx *EdgeContext) {}

// EnterPath is called when production path is entered.
func (s *BaseAgeListener) EnterPath(ctx *PathContext) {}

// ExitPath is called when production path is exited.
func (s *BaseAgeListener) ExitPath(ctx *PathContext) {}

// EnterValue is called when production value is entered.
func (s *BaseAgeListener) EnterValue(ctx *ValueContext) {}

// ExitValue is called when production value is exited.
func (s *BaseAgeListener) ExitValue(ctx *ValueContext) {}

// EnterProperties is called when production properties is entered.
func (s *BaseAgeListener) EnterProperties(ctx *PropertiesContext) {}

// ExitProperties is called when production properties is exited.
func (s *BaseAgeListener) ExitProperties(ctx *PropertiesContext) {}

// EnterPair is called when production pair is entered.
func (s *BaseAgeListener) EnterPair(ctx *PairContext) {}

// ExitPair is called when production pair is exited.
func (s *BaseAgeListener) ExitPair(ctx *PairContext) {}

// EnterArr is called when production arr is entered.
func (s *BaseAgeListener) EnterArr(ctx *ArrContext) {}

// ExitArr is called when production arr is exited.
func (s *BaseAgeListener) ExitArr(ctx *ArrContext) {}
