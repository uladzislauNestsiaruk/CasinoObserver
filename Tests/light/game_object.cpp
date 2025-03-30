#include "SFML/Graphics/Rect.hpp"
#include "SFML/System/Err.hpp"
#include "SFML/System/Vector2.hpp"
#include <animations_manager.hpp>
#include <game_object/game_object.hpp>
#include <game_object/game_objects_loader.hpp>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <memory>
#include <optional>
#include <ostream>
#include <stdexcept>
#include <textures_loader.hpp>
#include <unordered_map>

using object_ptr = std::shared_ptr<GameObject>;

const std::string kTestGameObject = GetAssetPrefix() + "game_objects/test_object.json";

class GameObjectTestFixture : public ::testing::Test {
protected:
    static void SetUpTestSuite() { Preload(); }

    static void TearDownTestSuite() {}

    void SetUp() override {
        test_object = ParseGameObjects(kTestGameObject, animations_manager_)[0];
    }

public:
    AnimationsManager animations_manager_;
    object_ptr test_object;
};

/*
 *   Constructots
 */
TEST(GameObject, Spritless_constructors) {
    AnimationsManager animations_manager;
    GameObject test_object("test", "afk", animations_manager);

    ASSERT_EQ(test_object.GetTag(), "test");
    ASSERT_EQ(test_object.GetActivePhase(), "afk");
    ASSERT_EQ(test_object.GetPosition(), sf::Vector2f(0, 0));
    ASSERT_EQ(test_object.GetScale(), sf::Vector2f(1, 1));
    ASSERT_EQ(test_object.GetSize(), sf::Vector2f(0, 0));
    ASSERT_EQ(test_object.GetVisibleRect(), sf::IntRect(0, 0, 0, 0));

    ASSERT_THROW(test_object.SetVisibleRect(sf::IntRect(0, 0, 1, 1)), std::logic_error);
    ASSERT_THROW(test_object.SetScale(sf::Vector2f(2, 2)), std::logic_error);
}

/*
 *   Move
 */

TEST(GameObject, Move_operation_basics) {
    AnimationsManager animations_manager;
    GameObject test_object("test", "afk", animations_manager);

    test_object.Move(sf::Vector2f(52, 52));
    ASSERT_EQ(test_object.GetPosition(), sf::Vector2f(52, 52));
}

TEST_F(GameObjectTestFixture, Move_operation_influence_on_subtree) {
    sf::Vector2f move_offset(52, 52);
    test_object->Move(move_offset);

    ASSERT_EQ(test_object->FindGameObjectByTag("a")->GetPosition(), test_object->GetPosition());
    ASSERT_EQ(test_object->FindGameObjectByTag("b")->GetPosition(), test_object->GetPosition());

    test_object->FindGameObjectByTag("a")->Move(move_offset);
    ASSERT_EQ(test_object->FindGameObjectByTag("a")->GetPosition(), move_offset + move_offset);
    ASSERT_EQ(test_object->FindGameObjectByTag("b")->GetPosition(),
              test_object->FindGameObjectByTag("a")->GetPosition());
    ASSERT_EQ(test_object->GetPosition(), move_offset);
}

/*
 *   Child lookup
 */

TEST_F(GameObjectTestFixture, looking_for_existing_child_in_game_object_tree) {
    object_ptr root = test_object->FindGameObjectByTag("root");
    object_ptr child_a = test_object->FindGameObjectByTag("a");
    object_ptr child_b = test_object->FindGameObjectByTag("b");

    ASSERT_TRUE(root);
    ASSERT_EQ(root->GetTag(), "root");

    ASSERT_TRUE(child_a);
    ASSERT_EQ(child_a->GetTag(), "a");

    ASSERT_TRUE(child_b);
    ASSERT_EQ(child_b->GetTag(), "b");
}

TEST_F(GameObjectTestFixture, looking_for_non_existing_child_in_game_object_tree) {
    object_ptr child_b = test_object->FindGameObjectByTag("b");

    object_ptr non_existing = test_object->FindGameObjectByTag("OBLAK52");
    object_ptr non_existing_parent = child_b->FindGameObjectByTag("a");
    object_ptr non_existing_root = child_b->FindGameObjectByTag("root");

    ASSERT_FALSE(non_existing);
    ASSERT_FALSE(non_existing_parent);
    ASSERT_FALSE(non_existing_root);
}

/*
 *   GameObject graph manipulation
 */

TEST_F(
    GameObjectTestFixture,
    Add_children_in_runtime_to_all_phases_Should_fall_if_child_with_such_name_exists_and_add_to_appropriate_parent_otherwise) {

    AnimationsManager animations_manager;
    object_ptr child_c = std::make_shared<GameObject>("c", "afk", animations_manager);
    object_ptr child_c_copy = std::make_shared<GameObject>("c", "afk", animations_manager);

    ASSERT_NO_THROW(test_object->AddChild(child_c, std::nullopt));
    ASSERT_TRUE(test_object->FindGameObjectByTag("c"));
    ASSERT_EQ(test_object->FindGameObjectByTag("c")->GetTag(), "c");

    ASSERT_THROW(test_object->AddChild(child_c_copy, std::nullopt), std::logic_error);
    ASSERT_THROW(test_object->FindGameObjectByTag("a")->AddChild(child_c_copy, std::nullopt),
                 std::logic_error);
    ASSERT_THROW(child_c->AddChild(child_c_copy, std::nullopt), std::logic_error);
}

TEST_F(GameObjectTestFixture, Remove_children_Shouldnt_fall_if_such_child_doesnt_exists) {
    AnimationsManager animations_manager;
    object_ptr child_c = std::make_shared<GameObject>("c", "afk", animations_manager);
    object_ptr child_d = std::make_shared<GameObject>("d", "afk", animations_manager);

    test_object->FindGameObjectByTag("a")->AddChild(child_c, std::nullopt);
    test_object->AddChild(child_d, std::nullopt);

    test_object->RemoveChild(child_c->GetTag(), std::nullopt);
    ASSERT_FALSE(test_object->FindGameObjectByTag(child_c->GetTag()));
    ASSERT_NO_THROW(child_d->AddChild(child_c, std::nullopt));

    test_object->RemoveChild(child_d->GetTag(), std::nullopt);
    ASSERT_FALSE(test_object->FindGameObjectByTag(child_d->GetTag()));
    ASSERT_FALSE(test_object->FindGameObjectByTag(child_c->GetTag()));

    ASSERT_NO_THROW(test_object->RemoveChild(child_c->GetTag(), std::nullopt));
    ASSERT_NO_THROW(test_object->RemoveChild(child_d->GetTag(), std::nullopt));
}

/*
 *   Resize
 */

sf::Vector2f operator*(sf::Vector2f lhs, sf::Vector2f rhs) {
    return sf::Vector2f(lhs.x * rhs.x, lhs.y * rhs.y);
}

sf::Vector2f operator/(sf::Vector2f lhs, sf::Vector2f rhs) {
    return sf::Vector2f(lhs.x / rhs.x, lhs.y / rhs.y);
}

std::ostream& operator<<(std::ostream& os, sf::Vector2f value) {
    os << "x: " << value.x << " y:" << value.y;
    return os;
}

TEST_F(GameObjectTestFixture,
       Resize_root_game_object_with_sprites_Should_resize_whole_subtree_with_proportions_saved) {
    sf::Vector2f test_size = sf::Vector2f(960, 640);

    std::unordered_map<std::string, std::string> parents;
    std::unordered_map<std::string, sf::Vector2f> initial_size;
    std::unordered_map<std::string, sf::Vector2f> initial_scale;
    sf::Vector2f initial_position = test_object->GetPosition();

    parents["root"] = "root";
    parents["a"] = "root";
    parents["b"] = "a";

    initial_size["root"] = test_object->GetSize();
    initial_size["a"] = test_object->FindGameObjectByTag("a")->GetSize();
    initial_size["b"] = test_object->FindGameObjectByTag("b")->GetSize();

    initial_scale["root"] = test_object->GetScale();
    initial_scale["a"] = test_object->FindGameObjectByTag("a")->GetScale();
    initial_scale["b"] = test_object->FindGameObjectByTag("b")->GetScale();

    sf::Vector2f scaling_factor = sf::Vector2f(test_size.x / test_object->GetSize().x,
                                               test_size.y / test_object->GetSize().y);
    test_object->Resize(test_size);

    for (const auto& parent_pair : parents) {
        sf::Vector2f expected_scale =
            initial_scale[parent_pair.first] / initial_scale[parent_pair.second] *
            test_object->FindGameObjectByTag(parent_pair.second)->GetScale();
        sf::Vector2f actual_scale = test_object->FindGameObjectByTag(parent_pair.first)->GetScale();
        ASSERT_EQ(actual_scale, expected_scale);

        sf::Vector2f expected_size =
            expected_scale * initial_size[parent_pair.first] / initial_scale[parent_pair.first];
        sf::Vector2f actual_size = test_object->FindGameObjectByTag(parent_pair.first)->GetSize();
        ASSERT_EQ(expected_size, actual_size);

        sf::Vector2f actual_position =
            test_object->FindGameObjectByTag(parent_pair.first)->GetPosition();
        ASSERT_EQ(initial_position, actual_position);
    }
}
