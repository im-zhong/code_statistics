// 2023/8/17
// zhangzhong
// code statistics

use code_statistics;

use clap::Parser;
use clap::ValueEnum;

#[derive(Debug, Copy, Clone, PartialEq, Eq, PartialOrd, Ord, ValueEnum)]
enum Language {
    C,
    Cpp,
    Rust,
    Python,
    All,
}

#[derive(Parser, Debug)]
#[command(name = "code_statistics")]
#[command(author = "zhangzhong")]
#[command(version = "0.1.0")]
#[command(about = "code statistics")]
struct Args {
    // Positional argument
    // cs -l,--languages <languages> -i,--ignore <ignored_paths>  <path>
    path: String,

    // 只能写成 -l=c,cpp,python
    // requires, optional
    // validation
    #[arg(short = 'l', long = "languages", value_enum, ignore_case = true)]
    #[arg(num_args=1.., value_delimiter=',')]
    languages: Vec<Language>,

    #[arg(short='i', long="ignore", num_args=1.., value_delimiter=',')]
    ignore_paths: Vec<String>,

    // flag
    #[arg(short, long)]
    verbose: bool,
    // Name of the person to greet
    // #[arg(short, long)]
    // name: String,

    // #[arg(short, long, default_value_t = 1)]
    // count: u8,
}

fn main() {
    let args = Args::parse();

    // for _ in 0..args.count {
    //     println!("Hello, {}!", args.name);
    // }
    // println!("{:?}", args);
    // for lan in &args.languages {
    //     println!("languages: {:?}", lan);
    // }

    // for p in &args.ignore_paths {
    //     println!("ignore_paths: {:?}", p);
    // }

    let conf = code_statistics::Conf::new(
        args.languages.iter().map(|x| x.to_string()).collect(),
        args.ignore_paths.clone(),
    );

    let driver = code_statistics::Driver::new(conf);
    let result = driver.run(&args.path);
    code_statistics::display(result);
}
