import Drupal from 'drupal';
import once from 'once';

Drupal.behaviors.lightbox = {
  attach(context) {
    const triggers = once('lightbox-init', '.js-lightbox', context);
    let triggerUsed;
    triggers.forEach(trigger => {
      const lightbox = document.getElementById(
        trigger.getAttribute('aria-controls')
      );
      if (!lightbox) return;

      const closeButton = lightbox.querySelector('.js-lightbox__close');

      function handleKeydown(event) {
        const { key } = event;
        if (key === 'Escape') {
          // eslint-disable-next-line no-use-before-define
          closeLightbox(event);
        }
      }

      function handleFocusIn(event) {
        // Keep the user from tabbing out of the modal.
        const focusable = Array.from(
          lightbox.querySelectorAll(
            'button, [href], input, select, textarea, iframe'
          )
        ).filter(item => item.tabIndex !== -1);
        if (!lightbox.contains(event.target)) {
          event.preventDefault();
          event.stopImmediatePropagation();
          focusable[0].focus();
        }
      }

      function closeLightbox(event) {
        event.preventDefault();
        const videoIFrame = lightbox.querySelector('iframe');
        if (videoIFrame && videoIFrame.hasAttribute('src')) {
          videoIFrame.setAttribute('data-src', videoIFrame.getAttribute('src'));
          videoIFrame.removeAttribute('src');
        }
        lightbox.classList.add('u-hidden');
        if (triggerUsed) {
          triggerUsed.focus();
          triggerUsed = null;
        }
        document.documentElement.classList.add('has-open-lightbox');
        lightbox.dispatchEvent(new Event('lightbox-close', { bubbles: true }));
        window.removeEventListener('keydown', handleKeydown);
        window.removeEventListener('focusin', handleFocusIn);
      }

      function openLightbox(event) {
        const videoIFrame = lightbox.querySelector('iframe');
        if (videoIFrame && videoIFrame.hasAttribute('data-src')) {
          videoIFrame.setAttribute('src', videoIFrame.getAttribute('data-src'));
        }
        lightbox.classList.remove('u-hidden');
        triggerUsed = event.target;
        window.addEventListener('keydown', handleKeydown);
        window.addEventListener('focusin', handleFocusIn);
        document.documentElement.classList.add('has-open-lightbox');
        lightbox.dispatchEvent(new Event('lightbox-open', { bubbles: true }));
        closeButton.focus();
      }

      closeButton.addEventListener('click', closeLightbox);
      lightbox.addEventListener('close-lightbox', closeLightbox);
      trigger.addEventListener('click', openLightbox);
      lightbox.addEventListener('open-lightbox', openLightbox);
    });
  },
};
