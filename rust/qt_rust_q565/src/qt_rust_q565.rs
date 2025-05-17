#[cxx_qt::bridge]
pub mod qobject {

    // Import Qt Objects used
    unsafe extern "C++" {
        include!("cxx-qt-lib/qstring.h");
        /// An alias to the QString type
        type QString = cxx_qt_lib::QString;
    }
    unsafe extern "C++" {
        include!("cxx-qt-lib/qimage.h");
        /// An alias to the QImage type
        type QImage = cxx_qt_lib::QImage;
    }
    unsafe extern "C++" {
        include!("cxx-qt-lib/qbytearray.h");
        /// An alias to the QByteArray type
        type QByteArray = cxx_qt_lib::QByteArray;
    }
    unsafe extern "C++" {
        include!("cxx-qt-lib/qurl.h");
        /// An alias to the QUrl type
        type QUrl = cxx_qt_lib::QUrl;
    }
    // ANCHOR_END: book_qstring_import

    // ANCHOR: book_rustobj_struct_signature
    extern "RustQt" {
        // The QObject definition
        // We tell CXX-Qt that we want a QObject class with the name MyObject
        // based on the Rust struct QRustQ565Data.
        #[qobject]
        #[qml_element]
        #[namespace = "my_object"]
        type QRustQ565 = super::QRustQ565Data;
    }
    // ANCHOR_END: book_rustobj_struct_signature

    // ANCHOR: book_rustobj_invokable_signature
    extern "RustQt" {
        // Declare the invokable methods we want to expose on the QObject
        #[qinvokable]
        #[cxx_name = "loadImage"]
        fn load_image(self: &QRustQ565, image_path: &QUrl, img: &QByteArray);

        #[qinvokable]
        #[cxx_name = "loadFromData"]
        fn load_data(self: &QRustQ565,  width: u16, height: u16, rgb888_raw: &[u8]) -> Vec<u8>;
    }
}

// Declare included crates
use core::pin::Pin;
use cxx_qt_lib::QString;
use cxx_qt_lib::QUrl;
use cxx_qt_lib::QByteArray;
use std::fs;
use q565;

/// The Rust struct for the QObject
#[derive(Default)]
pub struct QRustQ565Data {
}

// Define the implementation for the functions in our Qt object
impl qobject::QRustQ565 {
    /// Print a log message with the given string and number
    pub fn load_image(&self, image_path: &QUrl, img_data: &QByteArray) {

        let length = img_data.len();
        if length > 0 {
            println!("Got raw data with size: '{length}'");
            let mut prefix = image_path.to_local_file_or_default();
            println!("Hi from Rust! Loaded image with path: '{prefix}'");
            let encoded_data = self.load_data(1200, 1200, img_data.as_ref());
            let e_length = encoded_data.len();
            println!("Got encoded data with size: '{e_length}'");
            //let array_data = &encoded_data[..];
            let suffix = QString::from("_rust.q565");
            prefix.append(&suffix);
            let rust_str = String::from(prefix);
            fs::write(&rust_str, &encoded_data).unwrap();
            println!("Saved At : '{rust_str}'");
        } else {
            println!("Hi from Rust! Failed to load: '{image_path}'");
        }
    }
    pub fn load_data (&self, width: u16, height: u16, rgb888_raw: &[u8]) -> Vec<u8> {
        let mut v = Vec::with_capacity(1024 * 1024);
        let mut vec: Vec<u16> = Vec::new();
        for x in (0..rgb888_raw.len()).step_by(3) {
            let r = (rgb888_raw[x + 0] as u32 * 249 + 1014) >> 11;
            let g = (rgb888_raw[x + 1] as u32 * 253 + 505) >> 10;
            let b = (rgb888_raw[x + 2] as u32 * 249 + 1014) >> 11;
            vec.push(((r as u16) << 11) | ((g as u16) << 5) | (b as u16))
        }
        let rgb565_raw: &[u16] = unsafe { std::slice::from_raw_parts(vec.as_ptr() as *const u16, vec.len()) };
        // Move to encode and write it out to a file
        q565::encode::Q565EncodeContext::encode_to_vec(
            width as u16,
            height as u16,
            rgb565_raw,
            &mut v
        );
        return v;
    }
}
