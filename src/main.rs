use antiq::core::node::Node;
use antiq::widget;
use antiq::window::Windowed;
use std::rc::Rc;

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let mut application_window = widget::ApplicationWindow::new();
    application_window.get_mut().set_title("Afterglow");

    let mut app = widget::Application::new();
    app.add_child(Rc::new(application_window));
    app.get().run()
}
