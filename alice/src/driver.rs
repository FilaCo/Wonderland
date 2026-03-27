use std::io::{BufRead, BufReader, BufWriter, Write, stdin, stdout};

use clap::Parser;

use crate::compiler::{Alice, Config, run_alice};

#[derive(Parser, Clone, Debug)]
#[command(version)]
pub struct AliceDriver {
    /// Input source file
    #[arg(default_value = "-")]
    pub input: String,
}

impl AliceDriver {
    pub fn run(self) {
        run_alice(Config::from(self), |alice| match alice.input() {
            "-" => repl(alice),
            _ => todo!(),
        })
    }
}

impl Default for AliceDriver {
    fn default() -> Self {
        AliceDriver::parse()
    }
}

impl From<AliceDriver> for Config {
    fn from(value: AliceDriver) -> Self {
        Self { input: value.input }
    }
}

fn repl(alice: &Alice) {
    let input = stdin();
    let mut reader = BufReader::new(input);
    let mut line = String::new();

    let mut output = stdout();

    loop {
        write!(&mut output, "🦊 >>> ").expect("unable to write prompt invitation");
        output.flush().expect("unable to flush output writer");

        line.clear();

        match reader.read_line(&mut line) {
            Ok(0) => break,
            Ok(_) => (), // TODO: impl
            Err(_) => break,
        }
    }
}
