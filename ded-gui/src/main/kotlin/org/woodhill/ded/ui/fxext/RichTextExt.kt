package org.woodhill.ded.ui.fxext
import com.sun.javafx.binding.BidirectionalBinding.bind
import javafx.beans.binding.Bindings
import javafx.beans.property.Property
import javafx.beans.property.SimpleStringProperty
import javafx.beans.value.ObservableValue
import javafx.event.EventTarget
import javafx.util.StringConverter
import org.fxmisc.richtext.CodeArea
import org.fxmisc.richtext.LineNumberFactory
import tornadofx.*

//public void start(Stage primaryStage) {
//    CodeArea codeArea = new CodeArea();
//
//    // add line numbers to the left of area
//    codeArea.setParagraphGraphicFactory(LineNumberFactory.get(codeArea));
//    codeArea.setContextMenu( new DefaultContextMenu() );
//


// public fun <T> javafx.event.EventTarget.textarea(property: javafx.beans.property.Property<T>, converter: javafx.util.StringConverter<T>, op: javafx.scene.control.TextArea.() -> kotlin.Unit /* = compiled code */): javafx.scene.control.TextArea { /* compiled code */ }
//
// public fun javafx.event.EventTarget.textarea(property: javafx.beans.value.ObservableValue<kotlin.String>, op: javafx.scene.control.TextArea.() -> kotlin.Unit /* = compiled code */): javafx.scene.control.TextArea { /* compiled code */ }
//
// public fun javafx.event.EventTarget.textarea(value: kotlin.String? /* = compiled code */, op: javafx.scene.control.TextArea.() -> kotlin.Unit /* = compiled code */): javafx.scene.control.TextArea { /* compiled code */ }


fun EventTarget.richTextArea(value: String? = null, op: RichTextExt.() -> Unit = {}) = RichTextExt().attachTo(this, op) {
    //if (value != null) it.text = value

    if (value != null) it.appendText(value)

}

fun EventTarget.richTextArea(kkproperty: ObservableValue<String>, op: RichTextExt.() -> Unit = {}) = richTextArea(kkproperty.value).apply {

    textProperty().onChange {
        (kkproperty as SimpleStringProperty).value = it
    }
    kkproperty.stringBinding(textProperty()) {
        this.textProperty().value
    }
    this.paragraphGraphicFactory = LineNumberFactory.get(this)
    op(this)
}

//fun <T> EventTarget.richTextArea(property: Property<T>, converter: StringConverter<T>, op: RichTextExt.() -> Unit = {}) = richTextArea().apply {
//    //textProperty().bindBidirectional(property, converter)
//    //property.bindBidirectional(property, converter)
//
//    //textProperty().bind(property)
//    println("richtextarea 3:")
//
//    ViewModel.register(textProperty(), property)
//    op(this)
//}


//public fun richTextArea(property: javafx.beans.value.ObservableValue<kotlin.String>,  op: CodeArea.() -> kotlin.Unit) : CodeArea {
//    val codeArea =  CodeArea()
//    codeArea.paragraphGraphicFactory = LineNumberFactory.get(codeArea)
//    return codeArea
//}

class RichTextExt(): CodeArea() {
    private val codeArea =  CodeArea()


    init {
        codeArea.paragraphGraphicFactory = LineNumberFactory.get(codeArea)

    }
}