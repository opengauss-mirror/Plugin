//go:build go1.18
// +build go1.18

// SPDX-License-Identifier: Apache-2.0

package parser

import (
	"testing"

	"github.com/antlr/antlr4/runtime/Go/antlr"
)

// FuzzAgeParser exercises the driver's actual AGE lexer and parser without a database.
func FuzzAgeParser(f *testing.F) {
	for _, seed := range []string{
		"", "null", "true", "-1.25e+3", "1.5::numeric", "NaN", "-Infinity",
		`"escaped\n\u4e2d\"text"`, `{"a":[1,false,null,{"b":"value"}]}`,
		`{"id":1,"label":"Person","properties":{}}::vertex`,
		`{"id":2,"label":"KNOWS","start_id":1,"end_id":3,"properties":{}}::edge`,
		`[{"id":1,"label":"Person","properties":{}}::vertex]::path`,
		`{"broken":[`, "\x00\xff", `"\uZZZZ"`,
	} {
		f.Add([]byte(seed))
	}
	f.Fuzz(func(t *testing.T, data []byte) {
		// Bound per-input parsing cost; syntax errors are expected fuzz inputs.
		if len(data) > 4096 {
			return
		}
		lexer := NewAgeLexer(antlr.NewInputStream(string(data)))
		lexer.RemoveErrorListeners()
		tokens := antlr.NewCommonTokenStream(lexer, antlr.TokenDefaultChannel)
		tokens.Fill()
		tokens.Seek(0)
		parser := NewAgeParser(tokens)
		parser.RemoveErrorListeners()
		parser.Ageout()
	})
}
