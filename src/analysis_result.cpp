/*
        2020/6/44 17:48
        zhong, ysu
*/

#include "code_analysis.h"
#include <sstream>
using namespace std;

std::string CodeAnalyzerBase::AnalysisResult::to_string() const {
  stringstream context;
  for (size_t i = 0; i < line_category.size(); i++) {
    context << i + 1 << " : "
            << line_category_to_string(
                   static_cast<LineCategory>(line_category[i]))
            << endl;
  }
  context << "path: " << path << endl;
  context << "file lines: " << line_count << endl;
  context << "code lines: " << code_count << endl;
  context << "valid rate: " << code_count / static_cast<double>(line_count)
          << endl;
  return context.str();
}

void CodeAnalyzerBase::AnalysisResult::clear() noexcept {
  path.clear();
  line_count = 0;
  code_count = 0;
  blank_count = 0;
  annotation_count = 0;
  line_category.clear();
}

void CodeAnalyzerBase::AnalysisResult::statistics() noexcept {
  line_count = line_category.size();
  for (const auto &category : line_category) {
    if (category & LineCategory::BLANK)
      ++blank_count;
    if (category &
        (LineCategory::BLOCK_ANNOTATION | LineCategory::LINE_ANNOTATION))
      ++annotation_count;
    if (category & LineCategory::CODE)
      ++code_count;
  }
}
