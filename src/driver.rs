// 2023/8/17
// zhangzhong
// driver

use crate::analyzer;
use crate::conf;

pub struct Driver {
    conf: conf::Conf,

    // 这个设计是不好的
    // 考量一种设计是否是好的 就假设添加一个合理的功能
    // 看看当前的设计如果要支持这个功能 要做出多大的改变
    // 如果analyzer需要认识所有的语言
    // 那么每当我们添加一种新的语言的支持 就需要同时改动 analyzer 和 driver
    // 这样的设计是不好的
    // analyzers: std::collections::HashMap<String, Box<dyn analyzer::Analyzer>>,

    // 另外一种好的设计是让Analyzer保存这些语言的analyzer
    // 我们只需要认识extension
    // 这样设计的目的就是添加一门新的语言 driver不需要做任何的改动
    analyzers: analyzer::Analyzers,
}

// list[{path: result}]
pub struct AnalysisResult {
    path: String,
    result: analyzer::AnalysisResult,
    language: String,
}

// pub method
impl Driver {
    pub fn new(conf: conf::Conf) -> Self {
        todo!()
    }

    pub fn run(&self) -> Vec<AnalysisResult> {
        todo!()

        // chain rule
        // get file => conf filter =>
        // get_analyzer(extension) => analyze => result
    }
}

// private method
impl Driver {
    fn get_analyzer(&self, extension: &str) -> Box<dyn analyzer::Analyzer> {
        todo!()
        // lazy singleton
        self.analyzers.get_analyzer(extension).unwrap()
    }

    fn analyze(&self, path: &str) -> analyzer::AnalysisResult {
        // todo!();

        // get extension => get_analyzer => analyze
        let extension = path.split('.').last().unwrap();
        self.get_analyzer(extension).analyze()
    }
}
