use cxx_qt_build::{CxxQtBuilder, QmlModule};

fn main() {
    CxxQtBuilder::new()
        .qml_module(QmlModule {
            uri: "com.pr.rust.q565",
            rust_files: &["src/qt_rust_q565.rs"],
            qml_files: &["qml/main.qml"],
            ..Default::default()
        })
        .build();
}
