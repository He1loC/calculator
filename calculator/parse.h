#pragma once

static TreeNode* program(void);
static TreeNode* stmt_seq(void);
static TreeNode* stmt_seq_tail(void);
static TreeNode* stmt(void);
static TreeNode* decal_stmt(void);
static TreeNode* assign_stmt(void);
static TreeNode* expr(void);
static TreeNode* expr_tail(float termVal);
static TreeNode* term(void);
static TreeNode* term_tail(float termTailInh);
static TreeNode* factor(void);
static TreeNode* number(void);
static TreeNode* write_stmt(void);

TreeNode* parse(void);