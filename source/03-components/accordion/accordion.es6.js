import Drupal from 'drupal';
import once from 'once';
import { slideDown, slideUp } from '../../06-utility/_slide.es6';

Drupal.behaviors.accordion = {
  attach(context) {
    const ACCORDION_CLASS = 'js-accordion';
    const ACCORDION_TOGGLE_CLASS = 'js-accordion-toggle';
    const ACCORDION_DRAWER_CLASS = 'js-accordion-drawer';
    const ACCORDION_SPEED = 250;

    const accordions = once('accordion-init', `.${ACCORDION_CLASS}`, context);

    const openAccordion = (accordion, button) => {
      if (button.getAttribute('aria-expanded') === 'false') {
        button.setAttribute('aria-expanded', 'true');
        const accordionSection = document.getElementById(
          button.getAttribute('aria-controls')
        );
        accordionSection.setAttribute('aria-expanded', 'true');
        slideDown(accordionSection, ACCORDION_SPEED);
      }
    };

    const closeAccordion = (accordion, button) => {
      if (button.getAttribute('aria-expanded') === 'true') {
        button.setAttribute('aria-expanded', 'false');
        const accordionSection = document.getElementById(
          button.getAttribute('aria-controls')
        );
        accordionSection.setAttribute('aria-expanded', 'false');
        slideUp(
          document.getElementById(button.getAttribute('aria-controls')),
          ACCORDION_SPEED
        );
      }
    };

    // Accessible Accordion Functionality
    // Based off example work from W3 best aria practices
    // https://www.w3.org/TR/wai-aria-practices-1.1/examples/accordion/accordion.html
    accordions.forEach(accordion => {
      // Allow for multiple accordion sections to be expanded at the same time
      const allowMultiple = accordion.hasAttribute('data-allow-multiple');
      // Allow for each toggle to both open and close individually
      const allowToggle = allowMultiple
        ? true
        : accordion.hasAttribute('data-allow-toggle');
      accordion.addEventListener('click', event => {
        // Set target differently depending on click vs. keydown
        // because the <span> inside <button> screws things up
        if (
          event.target.classList.contains(ACCORDION_TOGGLE_CLASS) ||
          event.target.parentElement.classList.contains(ACCORDION_TOGGLE_CLASS)
        ) {
          let target;
          // Set target based on click or keydown
          if (event.target.classList.contains(ACCORDION_TOGGLE_CLASS)) {
            target = event.target;
          } else {
            target = event.target.parentElement;
          }
          // Check if the current toggle is expanded.
          const isExpanded = target.getAttribute('aria-expanded') === 'true';
          const active = accordion.querySelector('[aria-expanded="true"]');

          // without allowMultiple, close the open accordion
          if (!allowMultiple && active && active !== target) {
            closeAccordion(
              document.getElementById(active.getAttribute('aria-controls')),
              active,
              allowToggle,
              true
            );
          }

          if (!isExpanded) {
            openAccordion(
              document.getElementById(target.getAttribute('aria-controls')),
              target,
              allowToggle,
              !allowMultiple
            );
          } else if (allowToggle && isExpanded) {
            closeAccordion(
              document.getElementById(target.getAttribute('aria-controls')),
              target,
              allowToggle,
              !allowMultiple
            );
          }

          event.preventDefault();
        }
      });

      // These are used to style the accordion when one of the buttons has focus
      accordion
        .querySelectorAll(`.${ACCORDION_TOGGLE_CLASS}`)
        .forEach(trigger => {
          trigger.addEventListener('focus', () => {
            accordion.classList.add('focus');
          });

          trigger.addEventListener('blur', () => {
            accordion.classList.remove('focus');
          });
        });

      // Minor setup: will set disabled state, via aria-disabled, to an
      // expanded/ active accordion which is not allowed to be toggled close
      if (!allowToggle) {
        // Get the first expanded/ active accordion
        const expanded = accordion.querySelector('[aria-expanded="true"]');

        // If an expanded/ active accordion is found, disable
        if (expanded) {
          expanded.setAttribute('aria-disabled', 'true');
        }
      }

      // Initiate accordions on page load
      const accordionItems = accordion.querySelectorAll('.js-accordion-item');
      accordionItems.forEach(item => {
        const drawer = item.querySelector(`.${ACCORDION_DRAWER_CLASS}`);
        const toggle = item.querySelector(`.${ACCORDION_TOGGLE_CLASS}`);
        // Close all accordion items that are not 'default-open'
        if (!item.hasAttribute('data-accordion-open')) {
          closeAccordion(drawer, toggle);
        }
        // Update toggle tabindex
        toggle.removeAttribute('tabindex');
        // Add attribute 'processed'
        item.setAttribute('accordion-processed', '');
      });
    });
  },
};
