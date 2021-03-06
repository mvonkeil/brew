/**
 *
 *  |_  _ _
 *  |_)| (/_VV
 *
 *  Copyright 2015-2018 Marcus v. Keil
 *
 *  Created on: 07.09.17
 *
 */

#include <gtest/gtest.h>

#include <brew/video/ShaderVariables.h>
#include <brew/core/Exception.h>

using namespace brew;

TEST(ShaderVariables, GetUndefinedVariable) {
    ShaderVariablesLayout def(false);
    EXPECT_THROW(def.getDefinition("foo"), NotFoundException);
}

TEST(ShaderVariables, DefineAndGetVariable) {
    ShaderVariablesLayout def(false);

    def.define<u8>("foo", 3);

    auto& varDef = def.getDefinition("foo");

    EXPECT_EQ( ShaderVariablesLayout::VarType::u8, varDef.getType() );
    EXPECT_EQ( "foo", varDef.getName() );
    EXPECT_EQ( 3, varDef.getNumElements() );
}

TEST(ShaderVariables, CorrectDefinitionOrder) {
    ShaderVariablesLayout def(false);

    def.define<u8>("foo");
    def.define<u8>("bar");
    def.define<u8>("baz");

    EXPECT_EQ(0, def.getDefinition("foo").getIndex());
    EXPECT_EQ(1, def.getDefinition("bar").getIndex());
    EXPECT_EQ(2, def.getDefinition("baz").getIndex());
}

TEST(ShaderVariables, DefinitionOrderAfterUndefine) {
    ShaderVariablesLayout def(false);

    def.define<u8>("foo");
    def.define<u8>("bar");
    def.define<u8>("baz");

    def.undefine("bar");

    EXPECT_EQ(0, def.getDefinition("foo").getIndex());
    EXPECT_EQ(1, def.getDefinition("baz").getIndex());

    EXPECT_THROW(def.getDefinition("bar"), NotFoundException);
}

TEST(ShaderVariables, RedefineWithDifferentType) {
    ShaderVariablesLayout def(false);

    def.define<u8>("foo");
    EXPECT_EQ(ShaderVariablesLayout::VarType::u8, def.getDefinition("foo").getType());

    def.define<Real>("foo");
    EXPECT_EQ(ShaderVariablesLayout::VarType::Real, def.getDefinition("foo").getType());
}

TEST(ShaderVariables, DefineArrayByDimension) {
    ShaderVariablesLayout def(false);

    def.define<u8>("foo", 32);

    EXPECT_EQ(32, def.getDefinition("foo").getNumElements());
}

TEST(ShaderVariables, IterateOverVariables) {
    ShaderVariablesLayout def(false);

    def.define<u8>("a", 0);
    def.define<u8>("b", 1);
    def.define<u8>("c", 2);
    def.define<u8>("d", 3);
    def.define<u8>("e", 4);

    u8 i=0;
    for(auto it : def) {
        EXPECT_EQ(i++, it.getNumElements());
    }
}

TEST(ShaderVariables, DefineComplexTypes) {
    ShaderVariablesLayout def(false);

    def.define<std::shared_ptr<Texture> >("foo");

    EXPECT_NO_THROW(def.getDefinition("foo"));
    EXPECT_EQ(ShaderVariablesLayout::VarType::Texture, def.getDefinition("foo").getType());
}
