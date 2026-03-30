#ifndef SKILL_H
#define SKILL_H
#include <string>
#include <vector>
#include <memory>
#include<string>
using namespace std;


class skill_node
{
public:

	skill_node* left;
	skill_node* right;

	string skill_name;
	int shield_amount = 0;
	int damage_amount = 0;
	int heal_amount = 0;
	int energy_cost = 0;
};

// 创建一个三层的 skill_node 树：
//      skill1
//     /      \
//  skill2    skill3
//  /  \       /  \
// s4  s5     s6  s7
// 返回根节点（skill1）
inline skill_node* create_skill_tree()
{
	// 第一层
	skill_node* n1 = new skill_node();

	// 第二层
	skill_node* n2 = new skill_node();
	skill_node* n3 = new skill_node();

	// 第三层（叶子)
	skill_node* n4 = new skill_node();
	skill_node* n5 = new skill_node();
	skill_node* n6 = new skill_node();
	skill_node* n7 = new skill_node();

	// 初始化名字
	n1->skill_name = "skill1";
	n2->skill_name = "skill2";
	n3->skill_name = "skill3";
	n4->skill_name = "skill4";
	n5->skill_name = "skill5";
	n6->skill_name = "skill6";
	n7->skill_name = "skill7";

	// 初始化数值字段（示例设为 0，可按需修改)
	skill_node* all[] = { n1,n2,n3,n4,n5,n6,n7 };
	for (skill_node* p : all) {
		p->shield_amount = 0;
		p->damage_amount = 0;
		p->heal_amount = 0;
		p->energy_cost = 0;
		p->left = nullptr;
		p->right = nullptr;
	}

	// 建立连接（只做到三行)
	n1->left = n2;
	n1->right = n3;

	n2->left = n4;
	n2->right = n5;

	n3->left = n6;
	n3->right = n7;

	return n1;
}

#endif