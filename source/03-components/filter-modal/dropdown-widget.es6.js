/* eslint-disable */
import Drupal from 'drupal';
import once from 'once';

/**
 * @file
 * Transforms links into a dropdown list.
 */
Drupal.facets = Drupal.facets || {};
Drupal.behaviors.facetsDropdownWidget = {
  attach: function (context, settings) {
    Drupal.facets.makeDropdown(context, settings);
  },
};

/**
 * Turns all facet links into a dropdown with options for every link.
 *
 * @param {object} context
 *   Context.
 * @param {object} settings
 *   Settings.
 */
Drupal.facets.makeDropdown = (context, settings) => {
  // Find all dropdown facet links and turn them into an option.
  const facetDropdownLinks = once(
    'facets-dropdown-transform',
    '.js-facets-dropdown-links',
    context
  );

  if (facetDropdownLinks.length > 0) {
    facetDropdownLinks.forEach(list => {
      const links = list.querySelectorAll('.facet-item a');
      const dropdown = document.createElement('select');

      // Preserve all attributes of the list.
      Array.from(list.attributes).forEach(attribute => {
        dropdown.setAttribute(attribute.nodeName, attribute.nodeValue);
      });

      // Remove the class which we are using for .once().
      dropdown.classList.remove('js-facets-dropdown-links');
      // Add dropdown classes.
      dropdown.classList.add(
        'facets-dropdown',
        'js-facets-widget',
        'js-facets-dropdown',
        'c-form-item__select'
      );
      const id = list.dataset.drupalFacetId;
      // Add aria-labelledby attribute to reference label.
      dropdown.setAttribute('id', `facet_${id}_label`);

      // Add empty text option first.
      const defaultOptionLabel =
        settings.facets.dropdown_widget[id]['facet-default-option-label'];
      const defaultOption = document.createElement('option');
      defaultOption.setAttribute('value', '');
      defaultOption.innerText = defaultOptionLabel;
      dropdown.appendChild(defaultOption);
      list.insertAdjacentHTML(
        'afterbegin',
        `<li class="default-option"><a href="${
          window.location.href.split('?')[0]
        }">${Drupal.checkPlain(defaultOptionLabel)}</a></li>`
      );

      let hasActive = false;
      links.forEach(link => {
        const isActive = link.classList.contains('is-active');
        const option = document.createElement('option');
        option.setAttribute(
          'value',
          `${dropdown.dataset.drupalFacetAlias}:${link.dataset.drupalFacetItemValue}`
        );
        Object.entries(link.dataset).forEach(arr => {
          const [key, value] = arr;
          option.dataset[key] = value;
        });

        if (isActive) {
          hasActive = true;
          // Set empty text value to this link to unselect facet.
          const defaultLink = list.querySelector('.default-option a');
          if (defaultLink) {
            defaultLink.setAttribute('href', link.getAttribute('href'));
          }
          option.selected = true;
          const deactivateLink = link.querySelector('.js-facet-deactivate');
          if (deactivateLink) {
            deactivateLink.parentNode.removeChild(deactivateLink);
          }
        }

        // Add hierarchy indicator in case hierarchy is enabled.
        let prefix = '';
        if (link.parentElement.matches('li.facet-item')) {
          for (
            let p = link.parentElement && link.parentElement.parentElement;
            p;
            p = p.parentElement
          ) {
            if (p.matches('li.facet-item')) {
              prefix += '-';
            }
          }
        }
        option.innerText = `${prefix} ${link.innerText.trim()}`;
        dropdown.appendChild(option);
      });

      // Clear facet selection on clear event.
      dropdown.addEventListener('filter-modal:clear', () => {
        if (!defaultOption.selected) {
          const selectedOption = dropdown.querySelector('option[selected]');
          if (selectedOption) {
            selectedOption.selected = false;
          }
          defaultOption.selected = true;
        }
      });

      // Select empty text option if no facet is active.
      if (!hasActive) {
        defaultOption.selected = true;
      }

      // Replace links with dropdown.
      list.insertAdjacentElement('afterend', dropdown);
      list.classList.add('u-hidden');
      Drupal.attachBehaviors(dropdown.parentNode, Drupal.settings);
    });
  }
};
