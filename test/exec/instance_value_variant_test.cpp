#include "exec/instance/instance_value_variant.hpp"

#include <gmock/gmock.h>

namespace cmsl
{
    namespace exec
    {
        namespace inst
        {
            namespace test
            {
                using ::testing::Eq;
                using ::testing::DoubleNear;

                using which_t = instance_value_variant::which_type;

                TEST(InstanceValueVariantTest, DefaultConstructor_StoresBoolFalse)
                {
                    instance_value_variant variant;
                    ASSERT_THAT(variant.which(), Eq(which_t::bool_));
                    EXPECT_THAT(variant.get_bool(), Eq(false));
                }

                TEST(InstanceValueVariantTest, Constructor_StoresPassedValue)
                {
                    {
                        instance_value_variant variant{ true };
                        ASSERT_THAT(variant.which(), Eq(which_t::bool_));
                        EXPECT_THAT(variant.get_bool(), Eq(true));
                    }
                    {
                        instance_value_variant variant{ int_t{ 42 } };
                        ASSERT_THAT(variant.which(), Eq(which_t::int_));
                        EXPECT_THAT(variant.get_int(), Eq(int_t{ 42 }));
                    }
                    {
                        instance_value_variant variant{ 42.0 };
                        ASSERT_THAT(variant.which(), Eq(which_t::double_));
                        EXPECT_NEAR(variant.get_double(), 42.0, 0.0001);
                    }
                    {
                        instance_value_variant variant{ std::string{ "42" } };
                        ASSERT_THAT(variant.which(), Eq(which_t::string));
                        EXPECT_THAT(variant.get_string_cref(), Eq("42"));
                    }
                    {
                        const auto test_version = version_value{ 1u, 2u, 3u, 4u };
                        instance_value_variant variant{ test_version };
                        ASSERT_THAT(variant.which(), Eq(which_t::version));
                        EXPECT_THAT(variant.get_version_cref(), Eq(test_version));
                    }
                }

                TEST(InstanceValueVariantTest, Copyable)
                {
                    instance_value_variant v1{ "42" };
                    auto v2 = v1;
                    ASSERT_THAT(v2.which(), Eq(which_t::string));
                    EXPECT_THAT(v2.get_string_cref(), Eq("42"));

                    instance_value_variant v3{ v1 };
                    ASSERT_THAT(v3.which(), Eq(which_t::string));
                    EXPECT_THAT(v3.get_string_cref(), Eq("42"));
                }

                TEST(InstanceValueVariantTest, Movable)
                {
                    instance_value_variant v1{ "42" };
                    instance_value_variant v2{ std::move(v1) };
                    ASSERT_THAT(v2.which(), Eq(which_t::string));
                    EXPECT_THAT(v2.get_string_cref(), Eq("42"));

                    instance_value_variant v3{ true };
                    v3 = std::move(v2);
                    ASSERT_THAT(v3.which(), Eq(which_t::string));
                    EXPECT_THAT(v3.get_string_cref(), Eq("42"));

                    instance_value_variant v4{ version_value{ 1u } };
                    v4 = std::move(v2);
                    ASSERT_THAT(v3.which(), Eq(which_t::string));
                    EXPECT_THAT(v3.get_string_cref(), Eq("42"));
                }

                TEST(InstanceValueVariantTest, SetValue)
                {
                    instance_value_variant variant;

                    variant.set_bool(true);
                    ASSERT_THAT(variant.which(), Eq(which_t::bool_));
                    EXPECT_THAT(variant.get_bool(), Eq(true));

                    variant.set_int(int_t{ 42 });
                    ASSERT_THAT(variant.which(), Eq(which_t::int_));
                    EXPECT_THAT(variant.get_int(), Eq(int_t{ 42 }));

                    variant.set_double( 42.0 );
                    ASSERT_THAT(variant.which(), Eq(which_t::double_));
                    EXPECT_NEAR(variant.get_double(), 42.0, 0.0001);

                    variant.set_string("42");
                    ASSERT_THAT(variant.which(), Eq(which_t::string));
                    EXPECT_THAT(variant.get_string_cref(), Eq("42"));

                    const auto test_version = version_value{ 1u, 2u, 3u, 4u };
                    variant.set_version(test_version);
                    ASSERT_THAT(variant.which(), Eq(which_t::version));
                    EXPECT_THAT(variant.get_version_cref(), Eq(test_version));
                }
            }
        }
    }
}
