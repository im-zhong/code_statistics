mod c_analyzer;
mod cpp_analyzer;
mod notebook_analyzer;
mod python_analyzer;
mod rust_analyzer;
mod utils;

#[derive(Debug, Copy, Clone, PartialEq, Eq, PartialOrd, Ord)]
pub enum LineCategory {
    Blank,
    LineComment,
    BlockComment,
    Code,
    BlockCommentHead,
    RawLiteral,
    RawLiteralHead,
    StringLiteral,
}

pub struct AnalysisResult {
    pub file_name: String,
    pub total_lines: usize,
    pub blank_lines: usize,
    pub line_comments: usize,
    pub block_comments: usize,
    pub codes: usize,
    pub block_comment_heads: usize,
    pub raw_literals: usize,
    pub raw_literal_heads: usize,
    pub string_literals: usize,
}

impl AnalysisResult {
    fn clear(&mut self) {
        self.total_lines = 0;
        self.blank_lines = 0;
        self.line_comments = 0;
        self.block_comments = 0;
        self.codes = 0;
        self.block_comment_heads = 0;
        self.raw_literals = 0;
        self.raw_literal_heads = 0;
        self.string_literals = 0;
    }

    fn new(file_name: String) -> Self {
        Self {
            file_name,
            total_lines: 0,
            blank_lines: 0,
            line_comments: 0,
            block_comments: 0,
            codes: 0,
            block_comment_heads: 0,
            raw_literals: 0,
            raw_literal_heads: 0,
            string_literals: 0,
        }
    }
}

pub trait Analyzer {
    fn analyze(&self) -> AnalysisResult;
}

// get analyzer
// 所以 analyzer应该由本模块提供给driver
// driver不应该构建analyzer
// driver也不需要知道我们实现了多少个driver
// eager 其实是不好的
// 应该弄成lazy的
pub fn make_analyzers() -> Vec<Box<dyn Analyzer>> {
    vec![
        Box::new(c_analyzer::CAnalyzer {}),
        Box::new(cpp_analyzer::CppAnalyzer {}),
    ]
}

pub struct Analyzers {
    analyzers: Vec<Box<dyn Analyzer>>,
}

impl Analyzers {
    pub fn new() -> Self {
        Self {
            analyzers: make_analyzers(),
        }
    }

    pub fn get_analyzer(&self, extension: &str) -> Option<&Box<dyn Analyzer>> {
        self.analyzers
            .iter()
            .find(|analyzer| analyzer.get_extension() == extension)
    }
}
