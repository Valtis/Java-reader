#pragma once
#include <memory>
bool StrCmp(std::shared_ptr<cp_info> &first, std::shared_ptr<cp_info> &second);
bool StrCmp(cp_info *first, cp_info *second);

bool StrCmp(std::shared_ptr<cp_info> &first, const std::string &str);
bool StrCmp(cp_info *first, const std::string &str);

void PrintStr(std::shared_ptr<cp_info> &cp);
void PrintStr(cp_info *cp);