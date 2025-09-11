import re

class JavaScriptGenerator:
    """
    Takes a script string containing CHTL JS placeholders and replaces them
    with generated, standard JavaScript code.
    """
    def __init__(self, registry):
        self.registry = registry

    def _replace_selector_ref(self, match):
        block_id = match.group(1)
        selector_data = self.registry.get(block_id)
        if selector_data and selector_data['type'] == 'selector':
            selector_text = selector_data['selector']
            if selector_text.startswith('#'):
                return f'document.getElementById("{selector_text[1:]}")'
            return f'document.querySelector("{selector_text}")'
        return f"// Error: selector ref '{block_id}' not found"

    def _replace_listen_ref(self, match):
        block_id = match.group(1)
        listen_data = self.registry.get(block_id)

        if not listen_data or listen_data.get('type') != 'listen':
            return f"// Error: Could not find listen block data for {block_id}"

        target_js = listen_data.get('target')
        handlers = listen_data.get('handlers', {})

        if not target_js:
            return f"// Error: No target found for listen block {block_id}"

        event_listeners = []
        for event, handler in handlers.items():
            event_listeners.append(f'{target_js}.addEventListener("{event}", {handler});')

        return "\n".join(event_listeners)

    def _replace_delegate_ref(self, match):
        block_id = match.group(1)
        delegate_data = self.registry.get(block_id)

        if not delegate_data or delegate_data.get('type') != 'delegate':
            return f"// Error: Could not find delegate block data for {block_id}"

        parent_target = delegate_data.get('parent_target')
        child_selectors = delegate_data.get('target_selectors', '')
        handlers = delegate_data.get('handlers', {})

        if not parent_target or not child_selectors:
            return f"// Error: Missing target for delegate block {block_id}"

        # Generate a listener for each event type
        output_js = []
        for event, handler in handlers.items():
            js = f"""
{parent_target}.addEventListener("{event}", (event) => {{
    const targetElement = event.target.closest('{child_selectors}');
    if (targetElement && {parent_target}.contains(targetElement)) {{
        ({handler})(event);
    }}
}});"""
            output_js.append(js)

        return "\n".join(output_js)

    def _replace_animate_ref(self, match):
        block_id = match.group(1)
        anim_data = self.registry.get(block_id, {})

        if anim_data.get('type') != 'animate':
            return f"// Error: Animation data for {block_id} not found"

        keyframes = anim_data.get('keyframes', '[]')
        options = anim_data.get('options', '{}')
        when_event = anim_data.get('when')

        # Per CHTL.md, the animated element is the parent of the script tag.
        # `document.currentScript.parentElement` is the standard way to get this.
        target_element_js = "document.currentScript.parentElement"

        animation_call = f"{target_element_js}.animate({keyframes}, {options})"

        if when_event:
            # If 'when' is specified, wrap the animation in an event listener.
            return f"""
{target_element_js}.addEventListener('{when_event}', () => {{
    {animation_call};
}});"""
        else:
            # If 'when' is not specified, run the animation immediately after the DOM is loaded.
            # We wrap it to ensure the script's parent element exists.
            return f"""
document.addEventListener('DOMContentLoaded', () => {{
    if ({target_element_js}) {{
        {animation_call};
    }}
}});"""

    def generate(self, modified_script: str) -> str:
        """
        Replaces all CHTL JS placeholders in the script string.
        """
        # Note: The order of replacement matters if placeholders could be nested.
        # For now, we assume they are not.

        # First, replace listen, delegate, and animate blocks
        script = re.sub(r'__animate_ref__\("([^"]+)"\)', self._replace_animate_ref, modified_script)
        script = re.sub(r'__delegate_ref__\("([^"]+)"\);?', self._replace_delegate_ref, script)
        script = re.sub(r'__listen_ref__\("([^"]+)"\);?', self._replace_listen_ref, script)

        # Then, replace selector references, which are expressions
        script = re.sub(r'__selector_ref__\("([^"]+)"\)', self._replace_selector_ref, script)

        return script
