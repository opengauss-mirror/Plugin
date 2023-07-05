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

// AgeListener is a complete listener for a parse tree produced by AgeParser.
type AgeListener interface {
	antlr.ParseTreeListener

	// EnterAgeout is called when entering the ageout production.
	EnterAgeout(c *AgeoutContext)

	// EnterVertex is called when entering the vertex production.
	EnterVertex(c *VertexContext)

	// EnterEdge is called when entering the edge production.
	EnterEdge(c *EdgeContext)

	// EnterPath is called when entering the path production.
	EnterPath(c *PathContext)

	// EnterValue is called when entering the value production.
	EnterValue(c *ValueContext)

	// EnterProperties is called when entering the properties production.
	EnterProperties(c *PropertiesContext)

	// EnterPair is called when entering the pair production.
	EnterPair(c *PairContext)

	// EnterArr is called when entering the arr production.
	EnterArr(c *ArrContext)

	// ExitAgeout is called when exiting the ageout production.
	ExitAgeout(c *AgeoutContext)

	// ExitVertex is called when exiting the vertex production.
	ExitVertex(c *VertexContext)

	// ExitEdge is called when exiting the edge production.
	ExitEdge(c *EdgeContext)

	// ExitPath is called when exiting the path production.
	ExitPath(c *PathContext)

	// ExitValue is called when exiting the value production.
	ExitValue(c *ValueContext)

	// ExitProperties is called when exiting the properties production.
	ExitProperties(c *PropertiesContext)

	// ExitPair is called when exiting the pair production.
	ExitPair(c *PairContext)

	// ExitArr is called when exiting the arr production.
	ExitArr(c *ArrContext)
}
